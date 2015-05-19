#include "ParticleFishTracker.h"

#include <utility> // std::move
#include <algorithm> // std::generate_n
#include <iterator> // std::back_inserter

#include <QPainter>

#include <opencv2/opencv.hpp>

#include "source/tracking/algorithm/algorithms.h"
#include "particlefilter/ParticleBrightnessObserver.h"
#include "particlefilter/GridParticleBuckets.h"

namespace {
    auto _ = Algorithms::Registry::getInstance().register_tracker_type<ParticleFishTracker>("Particle Fish Tracker");
}

/**
 * Predicate used by this algorithm to sort particles, highest to lowest score.
 */
struct compareReverseParticleScorePredicate {
  bool operator() (const Particle& p1, const Particle& p2) const {
    return p1.getScore() > p2.getScore();
  }
};

/**
* Constructs a new instance using the tracking and special particle tracker settings set in settings.
*/
ParticleFishTracker::ParticleFishTracker(Settings& settings, QWidget *parent)
    : TrackingAlgorithm(settings, parent)
    , _toolsWidget(std::make_shared<QFrame>())
    , _showOriginal(false)
    , _rng(123)
    , _max_score(0)
    , _min_score(0)
    , _params(parent, settings)
	, _preprocessor(_params)
	, _clusters(_params)
{
    initToolsWidget();
}

ParticleFishTracker::~ParticleFishTracker(void)
{
}

/**
* Does the main work, detecting tracked objects (fish) and building a history for those objects.
*/
void ParticleFishTracker::track(unsigned long, cv::Mat& frame) {
	// we are going to modify the particle list, make sure it's not drawn concurrently
	std::lock_guard<std::mutex> lock(_current_particles_access_mutex);

	try {
		//dont do nothing if we ain't got an image
		if(frame.empty())
			return;

		// TODO check if frameNumber is jumping -> should lead to reseed

		// (1) Preprocess frame
		_prepared_frame = _preprocessor.preProcess(frame);

		// (2) Resampling (importance resampling) or seeding
		if (_current_particles.empty()) {
			// TODO params for this algorithm in settings.
			seedParticles(_params.getNumParticles(), 0, 0, frame.cols, frame.rows);
		} else {
			ParticleBrightnessObserver observer(_prepared_frame, _params);
			_sum_scores = 0;
			for (Particle& p : _current_particles) {
				observer.score(p);
				if (p.getScore() > _max_score) {
					_max_score = p.getScore();
				}
				if (p.getScore() < _min_score) {
					_min_score = p.getScore();
				}
				_sum_scores += p.getScore();
			}
			// Resample
			// - Sort for better performance (big scores first)
			std::sort(_current_particles.begin(), _current_particles.end(), compareReverseParticleScorePredicate());
			// - importance resampling
			importanceResample();
		}

		// (3) Clustering
		// - Only use particles with high enough score
		std::vector<Particle> particles_high_scores;
		const float score_cutoff = _min_score + ((_max_score - _min_score) * .1f);
		std::copy_if(_current_particles.begin(), _current_particles.end(), std::back_inserter(particles_high_scores),
			[score_cutoff](const Particle& p) { return p.getScore() >= score_cutoff; });
		_clusters.cluster(particles_high_scores, _params.getNumberOfClusters());

		// (4) Store results in history
		// TODO
	} catch (const cv::Exception &exc) {
		emit notifyGUI(exc.what(), MSGS::FAIL);
	}
}

/**
* Creates a set of new particles based on the old ones.
* For each of the new particles an ancestor particle is chosen randomly from
* the old particles, weighted proportionally to the old particles' score. The
* new particle inherits all properties from its ancestor, but will be slightly
* moved randomly (gaussian) in all dimensions.
*/
void ParticleFishTracker::importanceResample() {
	GridParticleBuckets buckets(_params.getMaxParticlesPerBucket(), _prepared_frame.rows, _prepared_frame.cols, _params.getBucketSize(), _params.getBucketSize());
	// Make a copy and generate new particles.
	size_t random_new_particles = 0;
	std::vector<unsigned> cluster_counts(_clusters.centers().rows);
	std::vector<Particle> old_particles = std::move(_current_particles);
	_current_particles.clear();

	for (size_t i = 0; i < old_particles.size(); i++) {
		size_t index = 0;
		const float rand = _rng.uniform(0.f, _sum_scores);
		for (float position = 0; position + old_particles[index].getScore() < rand; ) {
			position += old_particles[index].getScore();
			++index;
		}
		Particle to_wiggle = old_particles[index];

		if (buckets.putInBucket(to_wiggle)) {
			wiggleParticle(to_wiggle);
			_current_particles.push_back(to_wiggle);
		} else {
			++random_new_particles;
		}
	}
	seedParticles(random_new_particles, 0, 0, _prepared_frame.cols, _prepared_frame.rows);
}

/**
* Moves a particle randomly (gaussian) in all its dimensions. Low-scoring
* particles are wiggled more. The wiggling is restricted to within the
* _prepared_frame's dimensions.
*/
void ParticleFishTracker::wiggleParticle(Particle& to_wiggle) {
	float wiggle_distance;
	if (_max_score != _min_score) {
		wiggle_distance = static_cast<float>(_params.getParticleWiggleDistance()
			* ((_max_score - to_wiggle.getScore()) / (_max_score - _min_score)));
	} else {
		wiggle_distance = static_cast<float>(_params.getParticleWiggleDistance());
	}
	to_wiggle.setX(to_wiggle.getX() + static_cast<float>(_rng.gaussian(wiggle_distance)));
	to_wiggle.setY(to_wiggle.getY() + static_cast<float>(_rng.gaussian(wiggle_distance)));
	cutParticleCoords(to_wiggle);
}

/**
* Restricts a particle's x and y coordinates to the current frame's size.
*/
void ParticleFishTracker::cutParticleCoords(Particle& to_cut) {
	if (to_cut.getX() < 0) {
		to_cut.setX(0);
	}
	if (to_cut.getX() >= _prepared_frame.cols) {
		to_cut.setX(_prepared_frame.cols-1);
	}
	if (to_cut.getY() < 0) {
		to_cut.setY(0);
	}
	if (to_cut.getY() >= _prepared_frame.rows) {
		to_cut.setY(_prepared_frame.rows-1);
	}
}

/**
* Fills the list of current particles (_current_particles) with num_particles
* uniformly distributed particles.
*/
void ParticleFishTracker::seedParticles(size_t num_particles, int min_x, int min_y, int max_x, int max_y) {
	_current_particles.reserve(_current_particles.size() + num_particles);
	std::generate_n(std::back_inserter(_current_particles), num_particles, [&]() {
		const int x = _rng.uniform(min_x, max_x);
		const int y = _rng.uniform(min_y, max_y);
		// TODO include random angle
		const float a = 0;
		return Particle(x, y, a, static_cast<int>(_current_particles.size() + 1));
	});
}

/**
* Draws the result of the tracking for the current frame.
*/

void ParticleFishTracker::paintOverlay(QPainter *painter)
{
	// no overlay if we want to see the original image
	if (_showOriginal) return;

	QPen pen;
	auto drawParticle = [&](const Particle &particle, const QColor &color)
	{
		const float x = particle.getX();
		const float y = particle.getY();
		const int size = 2;
		pen.setColor(color);
		painter->setPen(pen);
		painter->drawEllipse(QPoint(x, y), size, size);
	};

	// at this point the particle list must not be modified anymore
	std::lock_guard<std::mutex> lock(_current_particles_access_mutex);

	for (const Particle &particle : _current_particles)
	{
		if (_min_score >= _max_score)
		{
			drawParticle(particle, QColor(0, 255, 0));
		}
		else
		{
			// Scale the score of the particle to get a nice color based on score.
			unsigned scaled_score = static_cast<unsigned>((particle.getScore() - _min_score) / (_max_score - _min_score) * 220.f);
			drawParticle(particle, QColor(0, 30 + scaled_score, 0));
		}
	}
}

/**
* This algorithm has state, this can be used to reset that state. It will not
* delete the tracked objects' history.
*/
void ParticleFishTracker::reset() {
	_preprocessor.reset();
	_min_score = 0;
	_max_score = 0;
	_clusters.clear();
	// TODO reset more...?
}

std::shared_ptr<QWidget> ParticleFishTracker::getToolsWidget	()
{
    return _toolsWidget;
}

void ParticleFishTracker::initToolsWidget()
{
    QFormLayout *layout = new QFormLayout(_toolsWidget.get());
    _modeBut = new QPushButton("show Original!", _toolsWidget.get());
    layout->addRow(_modeBut);
	QObject::connect(this->_modeBut, SIGNAL(clicked()), this, SLOT(switchMode()));
}

void ParticleFishTracker::switchMode()
{
	_showOriginal = !_showOriginal;
	if (_showOriginal)
		_modeBut->setText("show Filter!");
	else
		_modeBut->setText("show Original!");
	emit update();
}

std::shared_ptr<QWidget> ParticleFishTracker::getParamsWidget()
{
	return _params.getParamsWidget();
}

#include "ParticleClusters.h"

#include <opencv2/opencv.hpp>

/**
* Creates a new clustering object, can be reused for any number of detection
* frames.
*/
ParticleClusters::ParticleClusters(ParticleParams& params)
	: _params(params)
	, _centers(),
	_labels()
{
}

ParticleClusters::~ParticleClusters(void)
{
}

/**
* Does the k-means clustering on the particles. Uses k=num_clusters.
*/
void ParticleClusters::cluster(const std::vector<Particle>& particles, unsigned num_clusters) {
	// Prepare arguments for cv::kmeans.
	cv::Mat_<float> data(static_cast<int>(particles.size()), 2);
	for (int i = 0; i < data.rows; i++) {
		data[i][0] = particles[i].getX();
		data[i][1] = particles[i].getY();
	}
	int k = static_cast<int>(std::min<size_t>(num_clusters, particles.size()));
	cv::Mat bestLabels;
	cv::TermCriteria criteria(cv::TermCriteria::EPS, 0, _params.getClusteringEpsilon());
	int attempts = 1;
	int flags = cv::KMEANS_PP_CENTERS;
	cv::Mat centers;

	// Do the work.
	cv::kmeans(data, k, bestLabels, criteria, attempts, flags, centers);

	// Store the results.
	_centers = centers;
	_labels = bestLabels;
}

/**
* Assuming cluster() has been called, returns the index of the cluster center
* (from the previous cluster() invocation) closest to particle.
*/
unsigned ParticleClusters::getClosestClusterIndex(const Particle& particle) {
	unsigned min_distance_center = 0;
	float min_distance = FLT_MAX;
	for (int i = 0; i < _centers.rows; i++) {
		Particle center_particle(_centers.at<int>(i, 0), _centers.at<int>(i, 1), 0, 0);
		float distance = static_cast<float>(center_particle.dist(particle));
		if (distance < min_distance) {
			min_distance = distance;
			min_distance_center = i;
		}
	}
	return min_distance_center;
}

/**
* Returns a matrix containing all cluster center (x,y) coordinates (rows) from
* the previous cluster() invocation.
*/
const cv::Mat ParticleClusters::centers() {
	return _centers;
}

/**
* Returns a matrix containing rows: (1) index of particle (2) index of that
* particle's cluster center in centers(). Uses the data  from the previous
* cluster() invocation.
*/
const cv::Mat ParticleClusters::labels() {
	return _labels;
}

/**
* Resets the state of this object, as if cluster() had never been called.
*/
void ParticleClusters::clear() {
	_centers = cv::Mat();
	_labels = cv::Mat();
}
#ifndef SimpleTracker_H
#define SimpleTracker_H

#include <opencv2/opencv.hpp>

#include "source/tracking/TrackingAlgorithm.h"
#include "TrackedFish.h"
#include "FishCandidate.h"

class SimpleTracker : public TrackingAlgorithm
{
	public:
		static const float	MAX_TRACK_DISTANCE_PER_FRAME;
		static const float	MAX_TRACK_DISTANCE;
		static const int	CANDIDATE_SCORE_THRESHOLD;
		static const int	MAX_NUMBER_OF_TRACKED_OBJECTS;

		SimpleTracker			( Settings & settings, QWidget *parent );
		virtual ~SimpleTracker	( void );
		void track				( ulong frameNumber, cv::Mat & frame );
		void reset				();
		void paint			( cv::Mat& image );

	private:
		cv::BackgroundSubtractorMOG2	_bg_subtractor;
		std::vector<TrackedFish>		_tracked_fish;
		std::vector<FishCandidate>		_fish_candidates;

	public slots:
		//mouse click and move events
		void mouseMoveEvent		( QMouseEvent * e );
		void mousePressEvent	( QMouseEvent * e );
		void mouseReleaseEvent	( QMouseEvent * e );
		void mouseWheelEvent	( QWheelEvent * e );

};

#endif

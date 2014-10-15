#ifndef LandmarkTracker_H
#define LandmarkTracker_H

#include "cv.h"
#include "source/settings/Settings.h"
#include "source/tracking/TrackingAlgorithm.h"


class LandmarkTracker : public TrackingAlgorithm
{
private:
		cv::Point _selectorRecStart;
		cv::Point _selectorRecEnd;
		bool _showSelectorRec;
		
		//KML
		void drawRectangle(cv::Mat image);
		cv::Rect box;



	public:
		LandmarkTracker	( Settings & settings,  QWidget *parent );
		~LandmarkTracker	( void );
		



		void track			(  ulong frameNumber, cv::Mat & frame );
		void paint			( cv::Mat& image );
		void reset			( );

		void defineROI( cv::Mat image );


	public slots:
		//mouse click and move events
		void mouseMoveEvent		( QMouseEvent * e );
		void mousePressEvent	( QMouseEvent * e );
		void mouseReleaseEvent	( QMouseEvent * e );
		void mouseWheelEvent	( QWheelEvent * e );
		
};

#endif
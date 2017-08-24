#ifndef BIOTRACKERTRACKINGALGORITHM_H
#define BIOTRACKERTRACKINGALGORITHM_H


#include "Interfaces/IModel/IModel.h"

#include "TrackerParameter.h"

#include <opencv2/opencv.hpp>
#include "Interfaces/IModel/IModelTrackingAlgorithm.h"
#include "Interfaces/IModel/IModelDataExporter.h"
#include "Model/TrackedComponents/TrackedElement.h"
#include "Model/TrackedComponents/TrackedTrajectory.h"
#include "Model/TrackingAlgorithm/imageProcessor/detector/blob/cvBlob/BlobsDetector.h"
#include "Model/TrackingAlgorithm/imageProcessor/preprocessor/ImagePreProcessor.h"
#include "Model/TrackingAlgorithm/NN2dMapper.h"
#include <iostream>

#include "Model/Network/TcpListener.h"

class BioTrackerTrackingAlgorithm : public IModelTrackingAlgorithm
{
    Q_OBJECT
public:
    BioTrackerTrackingAlgorithm(IModel* parameter, IModel* trajectory/*QObject *parent = 0, ITrackedComponentFactory *factory = 0*/);
	~BioTrackerTrackingAlgorithm();

	void setDataExporter(IModelDataExporter *exporter);

Q_SIGNALS:
    void emitCvMatA(std::shared_ptr<cv::Mat> image, QString name);
	void emitDimensionUpdate(int x, int y);
	void emitTrackingDone(uint framenumber);

    // ITrackingAlgorithm interface
public Q_SLOTS:
	void doTracking(std::shared_ptr<cv::Mat> image, uint framenumber) override; 

private:
	void refreshPolygon();

	std::vector<FishPose> getLastPositionsAsPose();
	void resetFishHistory(int noFish);

    TrackedTrajectory* _TrackedTrajectoryMajor;

	TrackerParameter* _TrackingParameter;
	IModelDataExporter *_exporter;

	TcpListener* _listener;
	std::vector<cv::Point2f> _polygon_cm;

	ImagePreProcessor _ipp;
	BlobsDetector _bd;
	std::shared_ptr<NN2dMapper> _nn2d;

	// background subtraction
	cv::Ptr<cv::BackgroundSubtractorMOG2> _pMOG;

	int _noFish;

	//std::ofstream _ofs;

	int _imageX;
	int _imageY;
};

#endif // BIOTRACKERTRACKINGALGORITHM_H

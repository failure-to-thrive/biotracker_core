/****************************************************************************
  **
  ** This file is part of the BioTracker Framework
  ** by Andreas Jörg
  **
  ****************************************************************************/

#ifndef CONTROLLERPLUGIN_H
#define CONTROLLERPLUGIN_H

#include "Interfaces/IController/IController.h"
#include "Interfaces/IBioTrackerPlugin.h"
#include "QThread"
#include "QQueue"
#include "QPoint"

enum EDIT { REMOVE_TRACK, REMOVE_TRACK_ID, REMOVE_ENTITY, ADD, MOVE, SWAP, FIX, VALIDATE, VALIDATE_ENTITY, ROTATE_ENTITY };

struct queueElement {
	EDIT type;
	QPoint pos;
	IModelTrackedTrajectory* trajectory0;
	IModelTrackedTrajectory* trajectory1;
	IModelTrackedComponent* element;
	bool toggle;
	int id;
	double angle;
	uint frameNumber;
};

/**
 * This is the controller class of the Plugin Loader Component. This component is responsible for loading and managing BioTracker Plugins
 * The ControllerPlugin class is responsible for loading and connecting to BioTracker Plugins.
 */
class ControllerPlugin : public IController {
	Q_OBJECT
  public:
	ControllerPlugin(QObject* parent = 0, IBioTrackerContext* context = 0, ENUMS::CONTROLLERTYPE ctr = ENUMS::CONTROLLERTYPE::NO_CTR);
	~ControllerPlugin();

	void addToPluginList(QString str);

    /**
     * (This function
     * This function delivers a file path of a Plugin to the IModel class PluginLoader. If the Plugin could be loaded,
     * the functions additionally requests the name of the Plugin and sets it as an entry into an IView widget in the MainWindow.
     */
    void loadPluginFromFileName(QString str);

    /**
     * This function hands the received cv::Mat pointer and the current frame number to the PluginLoader.
     */
    void sendCurrentFrameToPlugin(std::shared_ptr<cv::Mat> mat, uint number);

	void selectPlugin(QString str);

signals:
	void emitRemoveTrajectory(IModelTrackedTrajectory* trajectory);
	void emitRemoveTrajectoryId(int id);
	void emitRemoveTrackEntity(IModelTrackedTrajectory* trajectory, uint frameNumber);
	void emitValidateTrajectory(int id);
	void emitValidateEntity(IModelTrackedTrajectory* trajectory, uint frameNumber);
	void emitAddTrajectory(QPoint pos);
	void emitMoveElement(IModelTrackedTrajectory* element, uint frameNumber, QPoint pos);
	void emitSwapIds(IModelTrackedTrajectory* trajectory0, IModelTrackedTrajectory* trajectory1);
	void emitToggleFixTrack(IModelTrackedTrajectory* trajectory0, bool toggle);
	void emitEntityRotation(IModelTrackedTrajectory* trajectory0, double angle, uint frameNumber);

	void emitUpdateView();
	void signalCurrentFrameNumberToPlugin(uint frameNumber);

	// IController interface
  protected:
	void createModel() override;
	void createView() override;
	void connectModelToController() override;
	void connectControllerToController() override;

	void createPlugin();

	void connectPlugin();
	void disconnectPlugin();

  private Q_SLOTS:
	/**
	 *
	 * If Tracking is active and the tracking process was finished, the Plugin is able to emit a Signal that triggers this SLOT.
	 */
	void receiveTrackingDone();
	/**
	*
	* Receive command to remove a trajectory and put it in edit queue
	*/
	void  receiveRemoveTrajectory(IModelTrackedTrajectory* trajectory);
	/**
	*
	* Receive command to remove a trajectory by id and put it in edit queue
	*/
	void  receiveRemoveTrajectoryId(int id);
	/**
	*
	* Receive command to remove a track entity and put it in edit queue
	*/
	void  receiveRemoveTrackEntity(IModelTrackedTrajectory* trajectory, uint frameNumber);
	/**
	*
	* Receive command to add a trajectory and put it in edit queue
	*/
	void  receiveAddTrajectory(QPoint pos);
	/**
	*
	* Receive command to move a element in a trajectory and put it in edit queue
	*/
	void  receiveMoveElement(IModelTrackedTrajectory* trajectory, uint frameNumber, QPoint pos, int toMove);
	/**
	*
	* Receive command to swap two ID's and put it in edit queue
	*/
	void  receiveSwapIds(IModelTrackedTrajectory* trajectory0, IModelTrackedTrajectory* trajectory1);

	void  receiveValidateTrajectory(int id);

	void  receiveValidateEntity(IModelTrackedTrajectory* trajectory, uint frameNumber);

	void  receiveToggleFixTrack(IModelTrackedTrajectory* trajectory, bool toggle);

	void  receiveEntityRotation(IModelTrackedTrajectory* trajectory, double angle, uint frameNumber);

	void  receivePauseState(bool state);

	void  receiveCurrentFrameNumberToPlugin(uint frameNumber);



  private:
	void loadPluginsFromPluginSubfolder();

    IBioTrackerPlugin* m_BioTrackerPlugin;

	QQueue<queueElement> m_editQueue;

	QPointer< QThread >  m_TrackingThread;

	bool m_paused = true;

	uint m_currentFrameNumber = 0;



};

#endif // CONTROLLERPLUGIN_H

#pragma once

#include "Interfaces/IController/IController.h"
#include "Interfaces/IModel/IModelAreaDescriptor.h"
#include <QMouseEvent>
#include <QKeyEvent>

class ControllerCoreParameter : public IController
{
	Q_OBJECT
public:
	ControllerCoreParameter(QObject *parent = 0, IBioTrackerContext *context = 0, ENUMS::CONTROLLERTYPE ctr = ENUMS::CONTROLLERTYPE::COREPARAMETER);

signals:
	//...

	private slots:
	//...
	void updateView();
	//update track number
	void receiveTrackNumber(int number);

	public slots:
	void setCorePermission(std::pair<ENUMS::COREPERMISSIONS, bool> permission);

	// IController interface
protected:
	void createModel() override;
	void createView() override;
	void connectModelToController() override;
	void connectControllerToController() override;
private:
	//...

};
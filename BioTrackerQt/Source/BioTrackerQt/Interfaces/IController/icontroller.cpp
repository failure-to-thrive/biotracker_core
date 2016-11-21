#include "icontroller.h"
#include "Interfaces/IController/icontroller.h"
#include "Interfaces/IBioTrackerContext.h"


IController::IController(QObject *parent, IBioTrackerContext *context, ENUMS::CONTROLLERTYPE ctr) : QObject(parent),
    m_BioTrackerContext(context),
    m_ControllerType(ctr)
{

}

IBioTrackerContext *IController::getBioTrackerContext()
{
    return m_BioTrackerContext.data();
}

void IController::createComponents()
{
    createModel();
    createView();
    connectModelController();
}

void IController::connectComponents()
{
    connectController();
}

void IController::addView(IView *view) {
    m_View = view;
}

void IController::addModel(IModel *model) {
    m_Model = model;
}

IModel *IController::getModel() {
    return m_Model.data();
}

IView *IController::getView() {
    return m_View;
}

ENUMS::CONTROLLERTYPE IController::getControllerType()
{
    return m_ControllerType;
}
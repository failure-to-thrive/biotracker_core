#include "BioTrackerApp.h"

namespace BioTracker {
namespace Core {

BioTrackerApp::BioTrackerApp()
{

}

BioTrackerApp::BioTrackerApp(QOpenGLWidget *widget)
    :   m_facade(widget)
{
}

void BioTrackerApp::unknownError(const std::exception &err) const
{

}

void BioTrackerApp::notify(const std::string &message, const MSGS::MTYPE type) const
{

}

void BioTrackerApp::frameCalculated(const size_t frameNumber, const std::string &filename, const double currentFps) const
{

}


}
}


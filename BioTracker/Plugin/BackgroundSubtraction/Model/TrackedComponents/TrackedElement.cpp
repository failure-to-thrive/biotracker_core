#include "TrackedElement.h"
#include "QDebug"
#include "QRect"
#include "QBrush"
#include "QPainter"
#include "QtMath"

TrackedElement::TrackedElement(QObject *parent, QString name, int id) :
	IModelTrackedPoint(parent),
	_name(name),
	_id(id)
{
	_x = 0;
	_y = 0;
	//_time = std::chrono::steady_clock::time_point;
	_deg = 0;
	_rad = 0;
	_valid = false;
	_pressed = false;
	_fixed = false;
}

QString TrackedElement::getName()
{
	return _name;
}

void TrackedElement::setFishPose(FishPose p)
{
	_pose = p;
	_x = p.position_cm().x;
	_y = p.position_cm().y;
	_deg = p.orientation_deg();
	_rad = p.orientation_rad();
	_valid = true;
	Q_EMIT notifyView();
}

FishPose TrackedElement::getFishPose()
{
	return _pose;
}

void  TrackedElement::setX(float val) {
	_x = val;
	FishPose pnew(cv::Point(_x, _pose.position_cm().y), cv::Point(-1, -1), _rad, _deg, _pose.width(), _pose.height(), _pose.getScore());
	_pose = pnew;
};

void  TrackedElement::setY(float val) {
	_y = val;
	FishPose pnew(cv::Point(_pose.position_cm().x, _y), cv::Point(-1, -1), _rad, _deg, _pose.width(), _pose.height(), _pose.getScore());
	_pose = pnew;
};

//TODO for all setters from here -> why no _pose = pnew;?
void  TrackedElement::setRad(float r) {
	_rad = r;
	FishPose pnew(_pose.position_cm(), _pose.position_px(), _rad, _deg, _pose.width(), _pose.height(), _pose.getScore());

};

void  TrackedElement::setDeg(float d) {
	_deg = d;
	FishPose pnew(_pose.position_cm(), _pose.position_px(), _rad, _deg, _pose.width(), _pose.height(), _pose.getScore());
	_pose = pnew;
};

void  TrackedElement::setW(float w) {
	_w = w;
	FishPose pnew(_pose.position_cm(), _pose.position_px(), _rad, _deg, _w, _pose.height(), _pose.getScore());
}

void TrackedElement::setH(float h) {
	_h = h;
	FishPose pnew(_pose.position_cm(), _pose.position_px(), _rad, _deg, _pose.width(), _h, _pose.getScore());
}


void TrackedElement::setXpx(float val) {
	std::cout << "Stub..." << std::endl;
}

void TrackedElement::setYpx(float val) {
	std::cout << "Stub..." << std::endl;
}

void TrackedElement::setWpx(float w) {
	std::cout << "Stub..." << std::endl;
}

void TrackedElement::setHpx(float h) {
	std::cout << "Stub..." << std::endl;
}

float TrackedElement::getX() {
	return _pose.position_cm().x;
}

float TrackedElement::getY() {
	return _pose.position_cm().y;
}

float TrackedElement::getXpx() {
    return _pose.position_px().x;
}

float TrackedElement::getYpx() {
    return _pose.position_px().y;
}

float TrackedElement::getDeg() {
	return _pose.orientation_deg();
}

void TrackedElement::operate()
{
	qDebug() << "I am TrackedElement " << _name;
}

void TrackedElement::pressed()
{
	_pressed = true;
	Q_EMIT notifyView();

}

void TrackedElement::notPressed()
{
	_pressed = false;
	Q_EMIT notifyView();
}

bool TrackedElement::getPressedStatus()
{
	return _pressed;
}




#include <SFML/Graphics.hpp>

#include "camera.hpp"

using namespace std;
using namespace sf;

Camera::Camera()
{
	_scale = 1;
	_x = 0;
	_y = 0;
	_angle = 0;
}

double Camera::scale()
{
	return _scale;
}

double Camera::x()
{
	return _x;
}

double Camera::y()
{
	return _y;
}

double Camera::angle()
{
	return _angle;
}

void Camera::setCentre(double x, double y)
{
	_x = x;
	_y = y;
}

void Camera::moveUp(double distance)
{
    _x += distance * _scale * sin(radians());
    _y += distance * _scale * -cos(radians());
}

void Camera::moveDown(double distance)
{
    _x += distance * _scale * -sin(radians());
    _y += distance * _scale * cos(radians());
}

void Camera::moveLeft(double distance)
{
    _x += distance * _scale * -cos(radians());
    _y += distance * _scale * -sin(radians());
}

void Camera::moveRight(double distance)
{
    _x += distance * _scale * cos(radians());
    _y += distance * _scale * sin(radians());
}

double Camera::radians()
{
	return _angle * 0.0174533;
}

void Camera::zoomIn(double factor)
{
	_scale *= factor;
	if (_scale > 5) _scale = 5;
}

void Camera::zoomOut(double factor)
{
	_scale /= factor;
	if (_scale < 0.1) _scale = 0.1;
}

void Camera::rotate(double angleChange)
{
	_angle += angleChange;
	while (_angle > 180) _angle -= 360;
	while (_angle < -180) _angle += 360;
}

View Camera::view(int windowWidth, int windowHeight)
{
	_view.setSize(windowWidth * _scale, windowHeight * _scale);
    _view.setCenter(_x, _y);
    _view.setRotation(_angle);
    return _view;
}

View Camera::overlayView(int windowWidth, int windowHeight)
{
	_view.setSize(windowWidth, windowHeight);
    _view.setCenter(windowWidth / 2, windowHeight / 2);
    _view.setRotation(0);
	return _view;
}

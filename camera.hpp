#pragma once

#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

class Camera
{

	private:

		double _scale;
		double _x;
		double _y;
		double _angle;

		View _view;

	public:

		Camera();	// CONSTRUCTOR

		double scale();
		double x();
		double y();
		double angle();

		double radians();

		void setCentre(double x, double y);

		void moveUp(double distance);
		void moveDown(double distance);
		void moveLeft(double distance);
		void moveRight(double distance);

		void zoomIn(double factor);
		void zoomOut(double factor);
		void rotate(double angleChange);

		View view(int windowWidth, int windowHeight);
		View overlayView(int windowWidth, int windowHeight);

};



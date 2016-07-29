#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "EllipseView.h"
#include "LineView.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::views; // Use the views namespace so we can quickly access the shape view

class ShapeViewSampleApp : public App {
public:
	void setup() override;
	void update() override;
	void draw() override;

private:
	// Vector to store references to each circle - this could be any kind of container at this point
	std::vector<EllipseViewRef> mCircles;
	LineViewRef					mLine;
};

void ShapeViewSampleApp::setup() {
	for (int i = 0; i < 10; i++) {
		float radius = (float)i*10.0f;
		float xPos = (float)i * radius + radius;
		float yPos = (float)i * radius + radius*0.5f;

		// Create local circle (does not get m or s before name)
		EllipseViewRef circle = EllipseViewRef(new EllipseView());
		// The ShapeView has the option of being a variety of shapes - this is how we tell it we want it to be a circle. Pass the position and radius.
		circle->setup(radius);
		circle->setPosition(vec2(xPos, yPos));
		// Set the tint color of this one circle
		circle->setBackgroundColor(ColorA(1.0f, 0.0f, 0.0f, 1.0f));
		// Play around with other properties you can set
		circle->setScale(vec2(1.2));	// Set the scale (hit states perhaps, or animating on/off). 

		// Add circle to the vector of all circles
		mCircles.push_back(circle);
	}

	mLine = LineViewRef(new LineView());
	mLine->setup(vec2(300));
}

void ShapeViewSampleApp::update() {
}

void ShapeViewSampleApp::draw() {
	gl::clear(Color(0, 0, 0));

	for (auto circle : mCircles) {
		circle->drawScene();
	}

	mLine->drawScene();
}

CINDER_APP(ShapeViewSampleApp, RendererGl)
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "ShapeView.h" // Include the ShapeView header. If we want any other kind of views in our class, include those here as well. 

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
	std::vector<ShapeViewRef> mCircles;
};

void ShapeViewSampleApp::setup() {
	for (int i = 0; i < 10; i++) {
		float radius = (float)i*10.0f;
		float xPos = (float)i * radius + radius;
		float yPos = (float)i * radius + radius*0.5f;

		// Create local circle (does not get m or s before name)
		ShapeViewRef circle = ShapeViewRef(new ShapeView());
		// The ShapeView has the option of being a variety of shapes - this is how we tell it we want it to be a circle. Pass the position and radius.
		circle->createSolidCircle(vec2(xPos, yPos), radius);
		// Set the color of this one circle.
		circle->setColor(ColorA(1.0f, 0.0f, 0.0f, 0.5f));
		// Play around with other properties you can set
		circle->setAlpha(0.25f);		// You could set a new alpha (without setting the entire color) = animation helper
		circle->setScale(vec2(1.2));	// Set the scale (hit states perhaps, or animating on/off). 

		// Add circle to the vector of all circles
		mCircles.push_back(circle);
	}
}

void ShapeViewSampleApp::update() {
}

void ShapeViewSampleApp::draw() {
	gl::clear(Color(0, 0, 0));

	for (auto circle : mCircles) {
		circle->drawScene();
	}
}

CINDER_APP(ShapeViewSampleApp, RendererGl)
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "ShapeView.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::views;

class TestViewsApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

private:
	ShapeViewRef mCircle;

};

void TestViewsApp::setup(){
	mCircle = ShapeViewRef(new ShapeView());
	mCircle->createSolidCircle(vec2(100.0f), 20.0f);
	mCircle->setColor(ColorA(1.0f, 0.0f, 0.0f, 0.5f));
}

void TestViewsApp::mouseDown( MouseEvent event ){
}

void TestViewsApp::update(){
}

void TestViewsApp::draw(){
	gl::clear( Color( 0, 0, 0 ) ); 

	mCircle->drawScene();
}

CINDER_APP( TestViewsApp, RendererGl )

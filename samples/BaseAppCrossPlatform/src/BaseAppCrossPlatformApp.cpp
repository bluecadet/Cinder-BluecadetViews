#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BaseAppCrossPlatformApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void BaseAppCrossPlatformApp::setup()
{
}

void BaseAppCrossPlatformApp::mouseDown( MouseEvent event )
{
}

void BaseAppCrossPlatformApp::update()
{
}

void BaseAppCrossPlatformApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( BaseAppCrossPlatformApp, RendererGl )

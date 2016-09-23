#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CancelTouchSampleApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void CancelTouchSampleApp::setup()
{
}

void CancelTouchSampleApp::mouseDown( MouseEvent event )
{
}

void CancelTouchSampleApp::update()
{
}

void CancelTouchSampleApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( CancelTouchSampleApp, RendererGl )

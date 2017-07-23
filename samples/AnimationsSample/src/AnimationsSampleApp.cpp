#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class AnimationsSampleApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void AnimationsSampleApp::setup()
{
}

void AnimationsSampleApp::mouseDown( MouseEvent event )
{
}

void AnimationsSampleApp::update()
{
}

void AnimationsSampleApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( AnimationsSampleApp, RendererGl )

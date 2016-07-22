#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseApp.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::views;

class ViewTypesSampleApp : public BaseApp {
public:
	// optional overrides
	void setup() override;
	void update() override;
	void draw() override;
};

void ViewTypesSampleApp::setup() {
	BaseApp::setup(); // will set up settings, views and touches
}

void ViewTypesSampleApp::update() {
	BaseApp::update(); // will update views and touches
}

void ViewTypesSampleApp::draw() {
	BaseApp::draw(); // will draw views
}

// Point to prepareSettings here to apply settings
CINDER_APP(ViewTypesSampleApp, RendererGl, ViewTypesSampleApp::prepareSettings)

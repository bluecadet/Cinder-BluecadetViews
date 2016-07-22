#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseApp.h"
#include "BaseView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::views;

class ViewTypesSampleApp : public BaseApp {
public:
	void setup() override;
	void update() override;
	void draw() override;
};

void ViewTypesSampleApp::setup() {
	BaseApp::setup();
}

void ViewTypesSampleApp::update() {
	BaseApp::update();
}

void ViewTypesSampleApp::draw() {
	BaseApp::draw();
}

CINDER_APP(ViewTypesSampleApp, RendererGl, ViewTypesSampleApp::prepareSettings)

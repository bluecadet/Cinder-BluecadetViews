#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseApp.h"
#include "BaseView.h"
#include "SettingsManager.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::views;
using namespace bluecadet::utils;

class ViewTypesSampleApp : public BaseApp {
public:
	void setup() override;
	void update() override;
	void draw() override;
	static void prepareSettings(ci::app::App::Settings* settings);
};

void ViewTypesSampleApp::prepareSettings(ci::app::App::Settings* settings) {
	BaseApp::prepareSettings(settings);
}

void ViewTypesSampleApp::setup() {
	BaseApp::setup();

	auto view = BaseViewRef(new BaseView());
	view->setSize(vec2(100, 100));
	view->setPosition(vec2(100, 100));
	view->setBackgroundColor(ColorA(1.0f, 0, 0, 0.75f));

	mRootView->addChild(view);
}

void ViewTypesSampleApp::update() {
	BaseApp::update();
}

void ViewTypesSampleApp::draw() {
	BaseApp::draw();
}

CINDER_APP(ViewTypesSampleApp, RendererGl, ViewTypesSampleApp::prepareSettings)

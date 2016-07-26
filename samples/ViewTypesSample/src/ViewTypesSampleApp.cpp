#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseApp.h"
#include "BaseView.h"
#include "TouchView.h"
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
	view->setPosition(vec2(10, 10));
	view->setBackgroundColor(ColorA(1.0f, 0, 0, 1.0f));
	mRootView->addChild(view);

	auto touchView = TouchViewRef(new TouchView());
	touchView->setSize(view->getSize());
	touchView->setPosition(view->getPosition().value() + vec2(view->getWidth() + 10, 0));
	touchView->setBackgroundColor(ColorA(0, 1.0f, 0, 1.0f));
	touchView->mDidBeginTouch.connect([=](const bluecadet::touch::TouchEvent& e) { touchView->setAlpha(0.5f); });
	touchView->mDidEndTouch.connect([=](const bluecadet::touch::TouchEvent& e) { touchView->setAlpha(1.0f); });
	touchView->mDidTap.connect([=](const bluecadet::touch::TouchEvent& e) {
		touchView->getTimeline()->appendTo(&touchView->getScale(), vec2(1.5f), 0.3f);
		touchView->getTimeline()->appendTo(&touchView->getScale(), vec2(1.0f), 0.3f);
	});
	mRootView->addChild(touchView);
}

void ViewTypesSampleApp::update() {
	BaseApp::update();
}

void ViewTypesSampleApp::draw() {
	BaseApp::draw();
}

CINDER_APP(ViewTypesSampleApp, RendererGl, ViewTypesSampleApp::prepareSettings)

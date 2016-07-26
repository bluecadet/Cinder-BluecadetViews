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
	touchView->mDidBeginTouch.connect([=](const bluecadet::touch::TouchEvent& e) { touchView->resetAnimations(); touchView->setAlpha(0.5f); });
	touchView->mDidTap.connect([=](const bluecadet::touch::TouchEvent& e) { touchView->getTimeline()->apply(&touchView->getAlpha(), 1.0f, 0.3f); });
	mRootView->addChild(touchView);

	auto diamondTouchView = TouchViewRef(new TouchView());
	diamondTouchView->setSize(touchView->getSize());
	diamondTouchView->setDebugDrawTouchPath(true);
	diamondTouchView->setTouchPath([=]{
		ci::Path2d p;
		p.moveTo(50, 0);
		p.lineTo(100, 50);
		p.lineTo(50, 100);
		p.lineTo(0, 50);
		p.close();
		return p;
	}());
	diamondTouchView->setPosition(touchView->getPosition().value() + vec2(touchView->getWidth() + 10, 0));
	diamondTouchView->setBackgroundColor(ColorA(0, 0, 1.0f, 1.0f));
	diamondTouchView->mDidBeginTouch.connect([=](const bluecadet::touch::TouchEvent& e) { diamondTouchView->resetAnimations(); diamondTouchView->setAlpha(0.5f); });
	diamondTouchView->mDidTap.connect([=](const bluecadet::touch::TouchEvent& e) { diamondTouchView->getTimeline()->apply(&diamondTouchView->getAlpha(), 1.0f, 0.3f); });
	mRootView->addChild(diamondTouchView);
}

void ViewTypesSampleApp::update() {
	BaseApp::update();
}

void ViewTypesSampleApp::draw() {
	BaseApp::draw();
}

CINDER_APP(ViewTypesSampleApp, RendererGl, ViewTypesSampleApp::prepareSettings)

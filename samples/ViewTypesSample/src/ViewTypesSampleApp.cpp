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
	view->setPosition(vec2(100, 100));
	view->setBackgroundColor(ColorA(0.5f, 0.5f, 0.5f, 0.75f));
	view->setTransformOrigin(view->getSize().value() * 0.5f);
	mRootView->addChild(view);

	auto touchView = TouchViewRef(new TouchView());
	touchView->setDebugDrawTouchPath(true);
	touchView->setSize(view->getSize());
	touchView->setTransformOrigin(0.5f * touchView->getSize().value());
	touchView->setPosition(view->getPosition().value() + vec2(view->getWidth() + 10, 0));
	touchView->setBackgroundColor(ColorA(0, 1.0f, 0, 1.0f));
	touchView->mDidBeginTouch.connect([=](const bluecadet::touch::TouchEvent& e) { touchView->resetAnimations(); touchView->setScale(1.5f); });
	touchView->mDidEndTouch.connect([=](const bluecadet::touch::TouchEvent& e) { touchView->getTimeline()->apply(&touchView->getScale(), vec2(1.0f), 0.3f); });
	mRootView->addChild(touchView);

	auto diamondTouchView = TouchViewRef(new TouchView());
	diamondTouchView->setDebugDrawTouchPath(true);
	diamondTouchView->setSize(touchView->getSize());
	diamondTouchView->setTransformOrigin(0.5f * diamondTouchView->getSize().value());
	diamondTouchView->setTouchPath([]{
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
	diamondTouchView->mDidBeginTouch.connect([=](const bluecadet::touch::TouchEvent& e) { diamondTouchView->resetAnimations(); diamondTouchView->setScale(1.5f); });
	diamondTouchView->mDidEndTouch.connect([=](const bluecadet::touch::TouchEvent& e) { diamondTouchView->getTimeline()->apply(&diamondTouchView->getScale(), vec2(1.0f), 0.3f); });
	mRootView->addChild(diamondTouchView);
}

void ViewTypesSampleApp::update() {
	BaseApp::update();
}

void ViewTypesSampleApp::draw() {
	BaseApp::draw();
}

CINDER_APP(ViewTypesSampleApp, RendererGl(RendererGl::Options().msaa(4)), ViewTypesSampleApp::prepareSettings)

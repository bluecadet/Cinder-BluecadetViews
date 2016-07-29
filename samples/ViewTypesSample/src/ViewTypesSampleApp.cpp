#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseApp.h"
#include "BaseView.h"
#include "EllipseView.h"
#include "LineView.h"
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
	settings->setWindowSize(ivec2(1024, 768));
}

void ViewTypesSampleApp::setup() {
	BaseApp::setup();

	auto view = BaseViewRef(new BaseView());
	view->setSize(vec2(100, 100));
	view->setPosition(vec2(10, 10));
	view->setBackgroundColor(ColorA(1.0f, 0.5f, 0.5f, 0.75f));
	mRootView->addChild(view);

	auto ellipseView = EllipseViewRef(new EllipseView());
	ellipseView->setSize(vec2(150, 100)); // if width/height are equal you can also use setRadius()
	ellipseView->setPosition(ellipseView->getSize() * 0.5f + vec2(view->getPosition().value().x + view->getSize().x + 10.0f, 10)); // ellipse is drawn around 0,0; so offset by 50% width/height
	ellipseView->setBackgroundColor(ColorA(0.5f, 1.0f, 0.5f, 0.75f));
	mRootView->addChild(ellipseView);

	auto lineView = LineViewRef(new LineView());
	lineView->setEndPoint(vec2(100, 100));
	lineView->setLineColor(ColorA(1.0f, 0.0f, 1.0f, 0.75f));
	lineView->setLineWidth(2.0f);
	lineView->setPosition(vec2(ellipseView->getPosition().value().x + ellipseView->getSize().x + 10.0f, 10));
	mRootView->addChild(lineView);

	auto touchView = TouchViewRef(new TouchView());
	touchView->setDebugDrawTouchPath(true);
	touchView->setSize(vec2(200, 100));
	touchView->setPosition(view->getPosition().value() + vec2(0, view->getWidth() + 10));
	touchView->setTransformOrigin(0.5f * touchView->getSize());
	touchView->setBackgroundColor(ColorA(0, 1.0f, 0, 1.0f));
	touchView->mDidBeginTouch.connect([=](const bluecadet::touch::TouchEvent& e) { touchView->resetAnimations(); touchView->setScale(1.5f); });
	touchView->mDidEndTouch.connect([=](const bluecadet::touch::TouchEvent& e) { touchView->getTimeline()->apply(&touchView->getScale(), vec2(1.0f), 0.3f); });
	mRootView->addChild(touchView);

	auto diamondTouchView = TouchViewRef(new TouchView());
	diamondTouchView->setDebugDrawTouchPath(true);
	diamondTouchView->setSize(vec2(200, 100));
	diamondTouchView->setPosition(touchView->getPosition().value() + vec2(touchView->getWidth() + 10, 0));
	diamondTouchView->setTransformOrigin(0.5f * diamondTouchView->getSize());
	diamondTouchView->setup([]{
		ci::Path2d p;
		p.moveTo(50, 0);
		p.lineTo(100, 50);
		p.lineTo(50, 100);
		p.lineTo(0, 50);
		p.close();
		return p;
	}());
	diamondTouchView->setBackgroundColor(ColorA(0, 0, 1.0f, 1.0f));
	diamondTouchView->mDidBeginTouch.connect([=](const bluecadet::touch::TouchEvent& e) { diamondTouchView->resetAnimations(); diamondTouchView->setScale(1.5f); });
	diamondTouchView->mDidEndTouch.connect([=](const bluecadet::touch::TouchEvent& e) { diamondTouchView->getTimeline()->apply(&diamondTouchView->getScale(), vec2(1.0f), 0.3f); });
	mRootView->addChild(diamondTouchView);

	const float circleTouchRadius = 50.0f;
	auto circleTouchView = TouchViewRef(new TouchView());
	circleTouchView->setDebugDrawTouchPath(true);
	circleTouchView->setSize(vec2(200, 100));
	circleTouchView->setPosition(diamondTouchView->getPosition().value() + vec2(diamondTouchView->getWidth() + 10, 0));
	circleTouchView->setTransformOrigin(0.5f * circleTouchView->getSize());
	circleTouchView->setup(circleTouchRadius, vec2(circleTouchRadius));
	circleTouchView->setBackgroundColor(ColorA(0, 1.0f, 1.0f, 1.0f));
	circleTouchView->mDidBeginTouch.connect([=](const bluecadet::touch::TouchEvent& e) { circleTouchView->resetAnimations(); circleTouchView->setScale(1.5f); });
	circleTouchView->mDidEndTouch.connect([=](const bluecadet::touch::TouchEvent& e) { circleTouchView->getTimeline()->apply(&circleTouchView->getScale(), vec2(1.0f), 0.3f); });
	mRootView->addChild(circleTouchView);
}

void ViewTypesSampleApp::update() {
	BaseApp::update();
}

void ViewTypesSampleApp::draw() {
	BaseApp::draw();
}

CINDER_APP(ViewTypesSampleApp, RendererGl(RendererGl::Options().msaa(4)), ViewTypesSampleApp::prepareSettings)

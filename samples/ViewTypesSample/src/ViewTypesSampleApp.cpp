#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#include "bluecadet/core/BaseApp.h"
#include "bluecadet/views/BaseView.h"
#include "bluecadet/views/EllipseView.h"
#include "bluecadet/views/FboView.h"
#include "bluecadet/views/LineView.h"
#include "bluecadet/views/TouchView.h"
#include "bluecadet/views/StatsView.h"


using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::core;
using namespace bluecadet::views;
using namespace bluecadet::touch;

class ViewTypesSampleApp : public BaseApp {
public:
	void setup() override;
	void update() override;
	void draw() override;
	void handleKeyDown(ci::app::KeyEvent event);
	static void prepareSettings(ci::app::App::Settings* settings);
	
};

void ViewTypesSampleApp::prepareSettings(ci::app::App::Settings* settings) {
	SettingsManager::getInstance()->mWindowSize = ivec2(1280, 720);
	SettingsManager::getInstance()->mFullscreen = false;
	BaseApp::prepareSettings(settings);
}

void ViewTypesSampleApp::setup() {
	BaseApp::setup();

	ScreenLayout::getInstance()->setDisplaySize(getWindowSize());
	SettingsManager::getInstance()->getParams()->minimize();

	auto view = BaseViewRef(new BaseView());
	view->setSize(vec2(100, 100));
	view->setPosition(vec2(10, 10));
	view->setBackgroundColor(ColorA(1.0f, 0.5f, 0.5f, 0.75f));
	getRootView()->addChild(view);

	auto ellipseView = EllipseViewRef(new EllipseView());
	ellipseView->setSize(vec2(150, 100)); // if width/height are equal you can also use setRadius()
	ellipseView->setPosition(ellipseView->getSize() * 0.5f + vec2(view->getPosition().value().x + view->getSize().x + 10.0f, 10)); // ellipse is drawn around 0,0; so offset by 50% width/height
	ellipseView->setBackgroundColor(ColorA(0.5f, 1.0f, 0.5f, 0.75f));
	ellipseView->setSmoothness(1.0f); // the default is 1
	getRootView()->addChild(ellipseView);

	// test smoothness update
	getSignalUpdate().connect([=] { ellipseView->setSmoothness(50.0f * getMousePos().x / (float)getWindowWidth()); });


	auto parentTouch = TouchViewRef(new TouchView());
	parentTouch->setSize(vec2(200, 100));
	parentTouch->setPosition(vec2(ellipseView->getPosition().value().x + ellipseView->getSize().x + 10.0f, 10));
	parentTouch->setBackgroundColor(Color::white());
	parentTouch->mDidEndTouch.connect([=](const bluecadet::touch::TouchEvent& e) {
		auto s = (parentTouch->getAlpha() == 1.0f) ? 0.0f : 1.0f;
		parentTouch->resetAnimations();
		parentTouch->getTimeline()->apply(&parentTouch->getAlpha(), s, 0.3f);
	});
	getRootView()->addChild(parentTouch);

	auto lineView = LineViewRef(new LineView());
	lineView->setEndPoint(vec2(100, 100));
	lineView->setLineColor(ColorA(1.0f, 0.0f, 1.0f, 1.0f));
	lineView->setLineWidth(2.0f);
	lineView->setPosition(vec2(0, 0));
	parentTouch->addChild(lineView);

	auto touchView = TouchViewRef(new TouchView());
	touchView->setDebugDrawTouchPath(true);
	touchView->setSize(vec2(200, 100));
	touchView->setPosition(view->getPosition().value() + vec2(0, view->getWidth() + 10));
	touchView->setTransformOrigin(0.5f * touchView->getSize());
	touchView->setBackgroundColor(ColorA(0, 1.0f, 0, 1.0f));
	touchView->mDidBeginTouch.connect([=](const bluecadet::touch::TouchEvent& e) { touchView->resetAnimations(); touchView->setScale(1.5f); });
	touchView->mDidEndTouch.connect([=](const bluecadet::touch::TouchEvent& e) { touchView->getTimeline()->apply(&touchView->getScale(), vec2(1.0f), 0.3f); });
	getRootView()->addChild(touchView);

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
	getRootView()->addChild(diamondTouchView);

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
	getRootView()->addChild(circleTouchView);

	auto circleTouchPath = TouchViewRef(new TouchView());
	circleTouchPath->setup(circleTouchRadius);
	circleTouchPath->setDebugDrawTouchPath(true);
	circleTouchPath->setBackgroundColor(Color(1.0f, 1.0f, 1.0f));
	circleTouchPath->mDidEndTouch.connect([=](const bluecadet::touch::TouchEvent& e) { 
		auto s = (circleTouchPath->getScale().value() == vec2(1.0f)) ? 1.5f : 1.0f;
		circleTouchPath->resetAnimations();
		circleTouchPath->getTimeline()->apply(&circleTouchPath->getScale(), vec2(s), 0.3f).updateFn([=](){
			circleTouchPath->setTouchPath(circleTouchRadius * circleTouchPath->getScale().value().x);
		});
	});
	circleTouchPath->setPosition(circleTouchView->getPosition().value() + vec2(circleTouchView->getWidth() + circleTouchPath->getWidth()/2, 0));
	getRootView()->addChild(circleTouchPath);


	auto fboView = FboViewRef(new FboView());
	fboView->setup(vec2(200));
	fboView->setBackgroundColor(ColorA(0.0f, 0.0f, 1.0f, 0.5f));
	fboView->setPosition( circleTouchPath->getPosition().value() + vec2(100.0f));
	auto circleInsideFbo = EllipseViewRef(new EllipseView());
	circleInsideFbo->setup(100.0f, ColorA(0.8f, 0.2f, 0.2f, 1.0f));
	fboView->addChild(circleInsideFbo);
	circleInsideFbo->getTimeline()->apply(&circleInsideFbo->getScale(), vec2(3.0f), 2.0f).loop(true);
	getRootView()->addChild(fboView);


	auto cancelView = TouchViewRef(new TouchView());
	cancelView->setSize(vec2(100, 100));
	cancelView->setPosition(vec2(100, 250));
	cancelView->setBackgroundColor(ColorA(1.0f, 0.5f, 0.5f, 0.75f));

	cancelView->mDidBeginTouch.connect([=](const bluecadet::touch::TouchEvent& e) {
		cancelView->getTimeline()->apply(&cancelView->getScale(), vec2(2.0f), 0.3f);
	});
	cancelView->mDidEndTouch.connect([=](const bluecadet::touch::TouchEvent& e) {
		cancelView->getTimeline()->apply(&cancelView->getScale(), vec2(1.0f), 0.3f);
	});

	getRootView()->addChild(cancelView);


	auto statsView = StatsViewRef(new StatsView());
	statsView->addStat("FPS", [&] { return to_string(getAverageFps()); });
	statsView->setPosition(vec2(0, ScreenLayout::getInstance()->getAppHeight() - statsView->getHeight()));
	getRootView()->addChild(statsView);

	getWindow()->getSignalKeyDown().connect(std::bind(&ViewTypesSampleApp::handleKeyDown, this, std::placeholders::_1));
}

void ViewTypesSampleApp::handleKeyDown(ci::app::KeyEvent event) {
	switch (event.getCode()) {
		case KeyEvent::KEY_c:
			console() << "ViewTypesSampleApp::handleKeyDown KEY_c" << endl;

			for (auto child : getRootView()->getChildren()) {
				if(	dynamic_pointer_cast<TouchView>(child)) 
					dynamic_pointer_cast<TouchView>(child)->cancelTouches();
				else console() << "This view isn't a TouchView, stop trying to cancel it's touch." << endl;
			}

			break;
	}
}

void ViewTypesSampleApp::update() {
	BaseApp::update();
}

void ViewTypesSampleApp::draw() {
	BaseApp::draw();
}

CINDER_APP(ViewTypesSampleApp, RendererGl(RendererGl::Options().msaa(4)), ViewTypesSampleApp::prepareSettings)
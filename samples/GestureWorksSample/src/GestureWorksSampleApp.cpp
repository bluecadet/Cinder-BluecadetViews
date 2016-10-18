#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Rand.h"

#include <core/BaseApp.h>
#include <views/TouchView.h>

#include "gwc/GestureWorksCore.h"
#include "GestureWorksTouchPlugin.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::core;
using namespace bluecadet::views;
using namespace bluecadet::touch;

class GestureWorksSampleApp : public BaseApp {
public:
	static void prepareSettings(ci::app::App::Settings* settings);
	void setup() override;
	void update() override;
	void draw() override;

	TouchViewRef createTransformableView();
	void makeViewTransformable(TouchViewRef view, BaseViewRef anchor);
};

void GestureWorksSampleApp::prepareSettings(ci::app::App::Settings* settings) {
	SettingsManager::getInstance()->mDebugWindowSize = ivec2(1280, 720);
	SettingsManager::getInstance()->mFps = 60;
	SettingsManager::getInstance()->mDebugFullscreen = true;
	SettingsManager::getInstance()->mDebugBorderless = false;
	SettingsManager::getInstance()->mDebugDrawTouches = true;
	BaseApp::prepareSettings(settings);
}

void GestureWorksSampleApp::setup() {
	// configure app
	ScreenLayout::getInstance()->setDisplaySize(getWindowSize());
	ScreenLayout::getInstance()->setNumRows(1);
	ScreenLayout::getInstance()->setNumColumns(1);

	SettingsManager::getInstance()->getParams()->minimize();
	SettingsManager::getInstance()->getParams()->setSize(vec2(200, 200));

	BaseApp::setup();
	BaseApp::addTouchSimulatorParams();

	// configure gestures
	auto gesturePlugin = GestureWorksTouchPluginRef(new GestureWorksTouchPlugin());
	TouchManager::getInstance()->addPlugin(gesturePlugin);

	// configure views
	getRootView()->setBackgroundColor(Color::gray(0.5f));
	getRootView()->setSize(ScreenLayout::getInstance()->getAppSize());

	// create views
	for (int i = 0; i < 50; ++i) {
		auto view = createTransformableView();

		view->setScale(vec2(randFloat(0.5, 5.0f)));
		view->setPosition(vec2(randFloat(getWindowWidth()), randFloat(getWindowHeight())));
		view->setBackgroundColor(hsvToRgb(vec3(randFloat(), 1.0f, 1.0f)));
		view->setRotation(randFloat(glm::two_pi<float>()));

		float alphaA = randFloat(0.5, 1.0);
		float alphaB = randFloat(0.5, 1.0);
		float duration = randFloat(1.0f, 4.0f);

		view->setAlpha(alphaA);
		view->getTimeline()->apply(&view->getAlpha(), alphaA, alphaB, duration, easeInOutQuad).pingPong().loop().delay(randFloat(duration));

		getRootView()->addChild(view);
	}
}

TouchViewRef GestureWorksSampleApp::createTransformableView()
{
	auto view = TouchViewRef(new TouchView());
	view->setSize(vec2(100, 100));
	view->setPosition((vec2(ScreenLayout::getInstance()->getAppSize()) - view->getSize()) * 0.5f);
	view->setTransformOrigin(view->getSize() * 0.5f);
	view->setScale(vec2(2));
	view->setBackgroundColor(Color(0, 1, 1));
	view->setMultiTouchEnabled(true);

	auto anchor = BaseViewRef(new BaseView());
	anchor->setBackgroundColor(Color(1, 0, 0));
	anchor->setSize(vec2(10));
	anchor->setTransformOrigin(anchor->getSize() * 0.5f);
	anchor->setPosition(view->getTransformOrigin().value() - anchor->getSize() * 0.5f);
	view->addChild(anchor);

	anchor->dispatchAfter([=]() {
		anchor->dispatchEvent("THIS IS A TEST");
	}, randFloat(5.0f));

	view->addEventCallback([=](const ViewEvent & e) {
		console() << e.type << endl;
	}, "THIS IS A TEST");

	makeViewTransformable(view, anchor);

	return view;
}

void GestureWorksSampleApp::makeViewTransformable(TouchViewRef view, BaseViewRef anchor) {
	static int i = 0;
	view->mDidReceiveGesture.connect([=](const gwc::GestureEvent & event) {
		static string originalTransformOriginKey = "gesture_originalTransformOrigin";

		if (event.gesture_id != "n-rotate-and-scale") return;
		
		if (event.phase == 0) {
			view->moveToFront();

			const vec2 originalTransformOrigin = view->getTransformOrigin();
			view->setUserInfo(originalTransformOriginKey, originalTransformOrigin);

			const vec2 gesturePos = vec2(event.x, event.y);
			
			const vec2 localGesturePos = view->convertGlobalToLocal(gesturePos);
			view->setTransformOrigin(localGesturePos, true);

		} else if (event.phase == 1) {
			vec2 position = view->getGlobalPosition();
			vec2 scale = view->getScale();
			float rotation = glm::angle(view->getRotation().value());

			position.x += event.values.at("rotate_and_scale_dx");
			position.y += event.values.at("rotate_and_scale_dy");
			scale.x += event.values.at("rotate_and_scale_dsx") * 0.01f;
			scale.y += event.values.at("rotate_and_scale_dsy") * 0.01f;
			rotation += toRadians(event.values.at("rotate_and_scale_dtheta"));
			rotation = fmodf(rotation, glm::two_pi<float>());
			if (rotation < 0) rotation += glm::two_pi<float>();

			view->setGlobalPosition(position);
			view->setScale(scale);
			view->setRotation(rotation);
			view->setGlobalPosition(position);

		} else if (event.phase == 2) {
			if (view->hasUserInfo(originalTransformOriginKey)) {
				const vec2 origin = view->getUserInfo<vec2>(originalTransformOriginKey);
				view->setTransformOrigin(origin, true);
				view->removeUserInfo(originalTransformOriginKey);
			}
		}

		// update anchor
		anchor->setPosition(view->getTransformOrigin().value() - anchor->getSize() * 0.5f);
	});
}

void GestureWorksSampleApp::update() {
	BaseApp::update();
}

void GestureWorksSampleApp::draw() {
	BaseApp::draw();
}

CINDER_APP(GestureWorksSampleApp, RendererGl(RendererGl::Options().msaa(4)), GestureWorksSampleApp::prepareSettings);
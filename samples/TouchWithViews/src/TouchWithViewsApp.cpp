#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#include "TouchManager.h"
#include "drivers/MouseDriver.h"
#include "drivers/TuioDriver.h"

#include "Btn.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::views;
using namespace bluecadet::touch;
using namespace bluecadet::touch::drivers;

class TouchWithViewsApp : public App {
public:
	void setup() override;
	void update() override;
	void draw() override;

private:
	MouseDriver		mMouseDriver;
	TuioDriver		mTuioDriver;
	BaseViewRef		mRootView;
};

void TouchWithViewsApp::setup() {
	// Set this to false to keep track of touches that didn't land on views
	//TouchManager::getInstance()->setDiscardMissedTouches(false);

	// Setup touch connections for entire app
	mMouseDriver.connect();
	mTuioDriver.connect();

	// Base view that contains our buttons
	mRootView = BaseViewRef(new BaseView());

	// Create a buttons using the Btn class
	for (int i = 0; i < 10; ++i) {
		auto btn = BtnRef(new Btn());
		btn->setup();
		btn->setDebugDrawTouchPath(true);
		btn->setPosition(vec2(randFloat((float)getWindowWidth()), randFloat((float)getWindowHeight())));
		btn->setTint(Color::hex(randInt(0xffffff)));
		mRootView->addChild(btn);
	}
}

void TouchWithViewsApp::update() {
	// Update touch manager
	TouchManager::getInstance()->update(mRootView);

	TouchManager::getInstance()->mDidBeginTouch.connect([=](const bluecadet::touch::TouchEvent& event) {
		if (event.target) {
			event.target->setScale(vec2(2.0f));
		}
	});
	TouchManager::getInstance()->mDidEndTouch.connect([=](const bluecadet::touch::TouchEvent& event) {
		if (event.target) {
			event.target->setScale(vec2(1.0f));
		}
	});

	// Update our scene
	mRootView->updateScene(0);
}

void TouchWithViewsApp::draw() {
	gl::clear(Color(0, 0, 0));

	// Draw the button including any of the button children (if you've added any)
	mRootView->drawScene();

	// Draw the touch manager for debugging/learning where the touchable area is
	TouchManager::getInstance()->debugDrawTouches();
}

CINDER_APP(TouchWithViewsApp, RendererGl)

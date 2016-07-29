#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Timeline.h"
#include "cinder/Rand.h"

#include "TouchManager.h"
#include "drivers/MouseDriver.h"
#include "drivers/TuioDriver.h"

#include "BaseView.h"
#include "TouchView.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet;
using namespace bluecadet::touch;
using namespace bluecadet::touch::drivers;
using namespace bluecadet::views;

class NestedTransformsSampleApp : public App {
public:
	void setup() override;
	void keyDown(KeyEvent event);
	void update() override;
	void draw() override;

private:
	MouseDriver mMouseConnection;
	TuioDriver mTouchConnection;

	BaseViewRef mRootView;
	BaseViewRef mContainer;
	double mPreviousTime;
};

void NestedTransformsSampleApp::setup() {
	gl::enableVerticalSync(false);

	mPreviousTime = 0.0;

	mMouseConnection.connect();
	mTouchConnection.connect();
	
	// Helper colors
	ci::ColorA red(1.0f, 0.6f, 0.6f, 0.9f);
	ci::ColorA green(0.6f, 1.0f, 0.6f, 0.9f);
	ci::ColorA blue(0.6f, 0.6f, 1.0f, 0.9f);
	ci::ColorA magenta(1.0f, 0.6f, 1.0f, 0.9f);
	ci::ColorA cyan(0.6f, 1.0f, 1.0f, 0.9f);

	// Create centered root view that rotates around its center
	mRootView = BaseViewRef(new BaseView());
	mRootView->setPosition(vec2(getWindowCenter()));
	//mRootView->getTimeline()->apply(&mRootView->getRotation(), glm::angleAxis(0.25f * (float)M_PI, vec3(0, 0, 1)), 8, EaseInOutQuad()).pingPong(true).loop(true);
	mRootView->getTimeline()->apply(&mRootView->getScale(), vec2(2.0, 2.0), 4.0, EaseInOutQuad()).pingPong(true).loop(true);

	mContainer = BaseViewRef(new BaseView());
	mContainer->setSize(vec2(400, 400));
	mContainer->setBackgroundColor(red);
	mContainer->setPosition(vec2(-200, -200));
	mRootView->addChild(mContainer);

	auto button = TouchViewRef(new TouchView());
	button->setup(vec2(50, 50));
	button->setPosition(vec2(0, 50));
	mContainer->addChild(button);

	auto moveToFrontFn = [&](const bluecadet::touch::TouchEvent& touchEvent) {
		touchEvent.target->moveToFront();
	};

	{ // overlapping buttons
		auto a = TouchViewRef(new TouchView());
		a->setSize(vec2(50, 50));
		a->setBackgroundColor(blue);
		a->setPosition(vec2(0, 0));
		a->mDidTap.connect(moveToFrontFn);
		mContainer->addChild(a, 0);

		auto b = TouchViewRef(new TouchView());
		b->setSize(vec2(50, 50));
		b->setBackgroundColor(magenta);
		b->setPosition(vec2(30, 0));
		b->mDidTap.connect(moveToFrontFn);
		mContainer->addChild(b, 0);

		auto c = TouchViewRef(new TouchView());
		c->setSize(vec2(50, 50));
		c->setBackgroundColor(cyan);
		c->setPosition(vec2(60, 0));
		c->mDidTap.connect(moveToFrontFn);
		mContainer->addChild(c, 0);

		auto c1 = TouchViewRef(new TouchView());
		c1->setSize(vec2(25, 25));
		c1->setBackgroundColor(red);
		c1->setPosition(vec2(10, 10));
		c1->mDidTap.connect(moveToFrontFn);
		c->addChild(c1);

		auto c2 = TouchViewRef(new TouchView());
		c2->setSize(vec2(25, 25));
		c2->setBackgroundColor(blue);
		c2->setPosition(vec2(20, 10));
		c2->mDidTap.connect(moveToFrontFn);
		c->addChild(c2);
	}

	/*{ // lots of objects
		auto bigContainer = BaseViewRef(new BaseView());
		bigContainer->setPosition(vec2(getWindowSize()) * 0.5f);
		mRootView->addChild(bigContainer);
		timeline().apply(&mContainer->getRotation(), glm::angleAxis(randFloat(0, 2.0 * M_PI), vec3(0, 0, 1)), randFloat(4, 5), EaseInOutQuad()).pingPong(true).loop(true);
		timeline().apply(&mContainer->getPosition(), mContainer->getPosition().value() + vec2(50, -50), 3.0, EaseInOutQuad()).pingPong(true).loop(true);
		timeline().apply(&mContainer->getScale(), vec2(1.5, 1.5), 4.0, EaseInOutQuad()).pingPong(true).loop(true);

		auto offset = -bigContainer->getPosition().value();

		for (int i = 0; i < 5000; ++i) {
			auto item = RectButtonRef(new RectButton());
			item->setup(vec2(10, 10), ci::ColorA(randFloat(), randFloat(), randFloat(), 0.5));
			item->mDidTap.connect(moveToFrontFn);
			item->setPosition(vec2(randFloat(getWindowWidth()), randFloat(getWindowHeight())) + offset);
			mContainer->addChild(item);
			timeline().apply(&item->getPosition(), vec2(randFloat(getWindowWidth()), randFloat(getWindowHeight())) + offset, randFloat(5, 10), EaseInOutQuad()).pingPong(true).loop(true);
			timeline().apply(&item->getRotation(), glm::angleAxis(randFloat(0, 2.0 * M_PI), vec3(0, 0, 1)), randFloat(5, 10), EaseInOutQuad()).pingPong(true).loop(true);
			timeline().apply(&item->getScale(), vec2(1.5, 1.5), randFloat(5, 10), EaseInOutQuad()).pingPong(true).loop(true);
		}
	}*/
}

void NestedTransformsSampleApp::keyDown(KeyEvent event) {
	switch (event.getChar()) {
		case 'q':
		case 'Q':
			quit();
			break;
	}
}

void NestedTransformsSampleApp::update() {
	double time = getElapsedSeconds();
	double deltaTime = mPreviousTime == 0.0 ? 0.0 : time - mPreviousTime;
	mPreviousTime = time;

	TouchManager::getInstance()->update(mRootView);
	mRootView->updateScene(deltaTime);
}

void NestedTransformsSampleApp::draw() {
	gl::clear(Color(0, 0, 0));
	mRootView->drawScene();
	gl::drawString("Tap/click a rectangle to move it to the front", vec2(), ci::ColorA(1, 1, 1, 1), Font("Arial", 20.0f));
	gl::drawString(to_string(getAverageFps()), vec2(getWindowSize() - ivec2(100)), ci::ColorA(1, 0, 0, 1));
}

CINDER_APP(NestedTransformsSampleApp, RendererGl(RendererGl::Options().msaa(4)), [&](ci::app::App::Settings *settings) {
	settings->setFrameRate(30);
	settings->setMultiTouchEnabled();
	settings->setWindowSize(800, 800);
})

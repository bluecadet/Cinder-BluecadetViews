#include "BaseApp.h"
#include "SettingsManager.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::utils;

namespace bluecadet {
namespace views {

BaseApp::BaseApp() :
	ci::app::App(),
	mLastFrameTime(0),
	mRootView(views::BaseViewRef(new views::BaseView()))
{
}

BaseApp::~BaseApp() {
}

void BaseApp::prepareSettings(ci::app::App::Settings* settings) {
	// Init settings manager
	fs::path appSettingsPath = ci::app::getAssetPath("appSettings.json");
	SettingsManager::getInstance()->setup(appSettingsPath, settings);
}

void BaseApp::setup() {
	// Add screen layout
	mScreenLayout = ScreenLayoutViewRef(new ScreenLayoutView());
	mScreenLayout->setup();
	mRootView->addChild(mScreenLayout);

	// Set if screen layout will draw
	SettingsManager::getInstance()->mDebugDrawScreenLayout ? mScreenLayout->setAlpha(1.0f) : mScreenLayout->setAlpha(0.0f);

	// Set up settings
	if (SettingsManager::getInstance()->mShowMouse) {
		showCursor();
	}
	else {
		hideCursor();
	}

	// Set up graphics
	gl::enableVerticalSync(SettingsManager::getInstance()->mVerticalSync);
	gl::enableAlphaBlending();

	// Set up touches
	mMouseDriver.connect();
	mTuioDriver.connect();

	// Set app width and height based on screen layout
	SettingsManager::getInstance()->mAppWidth = mScreenLayout->getDisplayWidth()*mScreenLayout->getDisplayTotalRows();
	SettingsManager::getInstance()->mAppHeight = mScreenLayout->getDisplayHeight()*mScreenLayout->getDisplayTotalColumns();
}

void BaseApp::update() {
	const double currentTime = getElapsedSeconds();
	const double deltaTime = mLastFrameTime == 0 ? 0 : currentTime - mLastFrameTime;
	mLastFrameTime = currentTime;

	touch::TouchManager::getInstance()->update(mRootView);
	mRootView->updateScene(deltaTime);
}

void BaseApp::draw() {
	gl::clear(Color(0, 0, 0));
	mRootView->drawScene();
}

void BaseApp::keyDown(KeyEvent event) {

	switch (event.getCode()) {
	case KeyEvent::KEY_q: {
		quit();
		break;
	}
	case KeyEvent::KEY_KP_PLUS:
	case KeyEvent::KEY_KP_MINUS:
	case KeyEvent::KEY_PLUS:
	case KeyEvent::KEY_EQUALS:
	case KeyEvent::KEY_MINUS: {
		const auto code = event.getCode();
		const float dir = (code == KeyEvent::KEY_KP_PLUS || code == KeyEvent::KEY_PLUS || code == KeyEvent::KEY_EQUALS) ? 1.0f : -1.0f;
		const float speed = event.isShiftDown() ? 0.25f : 0.1f;
		const float targetScale = mRootView->getScale().value().x * (1.0f + dir * speed);
		scaleRootViewCentered(targetScale);
		break;
	}
	case KeyEvent::KEY_KP1: case KeyEvent::KEY_KP2: case KeyEvent::KEY_KP3: case KeyEvent::KEY_KP4: case KeyEvent::KEY_KP5: case KeyEvent::KEY_KP6: case KeyEvent::KEY_KP7:
	case KeyEvent::KEY_1: case KeyEvent::KEY_2: case KeyEvent::KEY_3: case KeyEvent::KEY_4: case KeyEvent::KEY_5: case KeyEvent::KEY_6: case KeyEvent::KEY_7: {
		int screenId = (event.getChar() - (int)'0') - 1; // parse int from char, make 0-based
		zoomToScreen(screenId);
		break;
	}
	case KeyEvent::KEY_UP: {		
		mRootView->setPosition( vec2(mRootView->getPosition().value().x, mRootView->getPosition().value().y += getWindowHeight() * (event.isShiftDown() ? 1.0f : 0.25f)) );
		break;
	}
	case KeyEvent::KEY_DOWN: {
		mRootView->setPosition(vec2(mRootView->getPosition().value().x, mRootView->getPosition().value().y -= getWindowHeight() * (event.isShiftDown() ? 1.0f : 0.25f)));
		break;
	}
	case KeyEvent::KEY_LEFT: {
		mRootView->setPosition(vec2(mRootView->getPosition().value().x += getWindowWidth() * (event.isShiftDown() ? 1.0f : 0.25f), mRootView->getPosition().value().y));
		break;
	}
	case KeyEvent::KEY_RIGHT: {
		mRootView->setPosition(vec2(mRootView->getPosition().value().x -= getWindowWidth() * (event.isShiftDown() ? 1.0f : 0.25f), mRootView->getPosition().value().y));
		break;
	}
	}

}


void BaseApp::zoomToScreen(const int& screenId) {
	if (SettingsManager::getInstance()->mDebugMode) {
		ci::Rectf bounds = mScreenLayout->getDisplayBounds(screenId);
		ci::vec2 winSize = getWindowSize();

		mRootView->setScale(vec2(mScreenLayout->getScaleToFitBounds(bounds, winSize)) );
		mRootView->setPosition(mScreenLayout->getTranslateToCenterBounds(bounds, winSize / mRootView->getScale().value().x));
	}
}

void BaseApp::scaleRootViewCentered(const float& targetScale) {
	const float currentScale = mRootView->getScale().value().x;

	vec2 windowSize = vec2(getWindowSize());
	vec2 currentSize = windowSize / currentScale;
	vec2 targetSize = windowSize / targetScale;

	mRootView->setScale(vec2(targetScale));
	mRootView->setPosition(mRootView->getPosition().value() += (targetSize - currentSize) * 0.5f);
}

}
}
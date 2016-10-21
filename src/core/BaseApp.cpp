#include "BaseApp.h"
#include "SettingsManager.h"
#include "ScreenLayout.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::views;
using namespace bluecadet::touch;
using namespace bluecadet::touch::drivers;

namespace bluecadet {
namespace core {

BaseApp::BaseApp() :
	ci::app::App(),
	mLastUpdateTime(0),
	mRootView(views::BaseViewRef(new views::BaseView()))
{
}

BaseApp::~BaseApp() {
}

void BaseApp::prepareSettings(ci::app::App::Settings *settings) {
	fs::path appSettingsPath = ci::app::getAssetPath("appSettings.json");
	SettingsManager::getInstance()->setup(appSettingsPath, settings);

	// Apply pre-launch settings
	settings->setConsoleWindowEnabled(SettingsManager::getInstance()->mConsoleWindowEnabled);
	settings->setFrameRate((float)SettingsManager::getInstance()->mFps);
	settings->setWindowSize(SettingsManager::getInstance()->mWindowSize);
	settings->setBorderless(SettingsManager::getInstance()->mBorderless);
	settings->setFullScreen(SettingsManager::getInstance()->mFullscreen);
	settings->setHighDensityDisplayEnabled(true);

	// Keep window top-left within display bounds
	if (settings->getWindowPos().x == 0 && settings->getWindowPos().y == 0) {
		ivec2 windowPos = (Display::getMainDisplay()->getSize() - settings->getWindowSize()) / 2;
		windowPos = glm::max(windowPos, ivec2(0));
		settings->setWindowPos(windowPos);
	}
}


void BaseApp::setup() {
	auto settings = SettingsManager::getInstance();

	// Set up screen layout
	int displayWidth = settings->hasField("settings.display.width") ? settings->getField<int>("settings.display.width") : ScreenLayout::getInstance()->getDisplayWidth();
	int displayHeight = settings->hasField("settings.display.height") ? settings->getField<int>("settings.display.height") : ScreenLayout::getInstance()->getDisplayHeight();
	int rows = settings->hasField("settings.display.rows") ? settings->getField<int>("settings.display.rows") : ScreenLayout::getInstance()->getNumRows();
	int cols = settings->hasField("settings.display.columns") ? settings->getField<int>("settings.display.columns") : ScreenLayout::getInstance()->getNumColumns();
	
	ScreenLayout::getInstance()->setup(ivec2(displayWidth, displayHeight), rows, cols);
	ScreenLayout::getInstance()->zoomToFitWindow();
	
	// Apply run-time settings
	if (settings->mShowMouse) {
		showCursor();
	} else {
		hideCursor();
	}

#if defined(CINDER_MSW)
	// Move window to foreground so that console output doesn't obstruct it
	HWND nativeWindow = (HWND)getWindow()->getNative();
	::SetForegroundWindow(nativeWindow);
#endif

	// Set up graphics
	gl::enableVerticalSync(settings->mVerticalSync);
	gl::enableAlphaBlending();

	// Set up touches
	//TouchManager::getInstance()->setDiscardMissedTouches(false);

	mMouseDriver.connect();
	mTuioDriver.connect();
	mSimulatedTouchDriver.setup(Rectf(vec2(0), getWindowSize()), 60);
}

void BaseApp::update() {
	const double currentTime = getElapsedSeconds();
	const double deltaTime = mLastUpdateTime == 0 ? 0 : currentTime - mLastUpdateTime;
	mLastUpdateTime = currentTime;

	// get the screen layout's transform and apply it to all
	// touch events to convert touches from window into app space
	const auto appTransform = glm::inverse(ScreenLayout::getInstance()->getTransform());
	const auto appSize = ScreenLayout::getInstance()->getAppSize();
	touch::TouchManager::getInstance()->update(mRootView, appSize, appTransform);
	mRootView->updateScene(deltaTime);
}

void BaseApp::draw(const bool clear) {
	auto settings = SettingsManager::getInstance();

	if (clear) {
		gl::clear(settings->mClearColor);
	}

	{
		gl::ScopedModelMatrix scopedMatrix;
		// apply screen layout transform to root view
		gl::multModelMatrix(ScreenLayout::getInstance()->getTransform());
		mRootView->drawScene();

		// draw debug touches in app coordinate space
		if (settings->mDebugMode && settings->mDrawTouches) {
			touch::TouchManager::getInstance()->debugDrawTouches();
		}
	}

	if (settings->mDebugMode) {
		// draw params in window coordinate space
		if (settings->mDrawScreenLayout) {
			ScreenLayout::getInstance()->draw();
		}
		settings->getParams()->draw();
	}
}

void BaseApp::keyDown(KeyEvent event) {
	if (event.isHandled()) {
		// don't do anything on previously handled events
		return;
	}

	switch (event.getCode()) {
		case KeyEvent::KEY_q:
			quit();
			break;

		case KeyEvent::KEY_f:
			SettingsManager::getInstance()->mFullscreen = !isFullScreen();
			setFullScreen(SettingsManager::getInstance()->mFullscreen);
			break;
	}
}

void BaseApp::addTouchSimulatorParams(float touchesPerSecond) {

	mSimulatedTouchDriver.setTouchesPerSecond(touchesPerSecond);

	const string groupName = "Touch-Sim";

	SettingsManager::getInstance()->getParams()->addParam<float>("Touches/s", [&](float v) {
		mSimulatedTouchDriver.setTouchesPerSecond(v);
	}, [&]() {
		return mSimulatedTouchDriver.getTouchesPerSecond();
	}).group(groupName);

	SettingsManager::getInstance()->getParams()->addParam<bool>("Stress Test", [&](bool v) {
		if (!mSimulatedTouchDriver.isRunning()){
			SettingsManager::getInstance()->mDrawTouches = true;
			mSimulatedTouchDriver.setBounds(Rectf(vec2(0), getWindowSize()));
			mSimulatedTouchDriver.start();
		} else {
			SettingsManager::getInstance()->mDrawTouches = false;
			mSimulatedTouchDriver.stop();
		}
	}, [&] {
		return SettingsManager::getInstance()->mDrawTouches && mSimulatedTouchDriver.isRunning();
	}).group(groupName);

	SettingsManager::getInstance()->getParams()->addButton("Stress Test Tap + Drag", [&] {
		SettingsManager::getInstance()->mDrawTouches = true;
		mSimulatedTouchDriver.setBounds(Rectf(vec2(0), getWindowSize()));
		mSimulatedTouchDriver.setMinTouchDuration(0);
		mSimulatedTouchDriver.setMaxTouchDuration(1.f);
		mSimulatedTouchDriver.setMaxDragDistance(200.f);
		mSimulatedTouchDriver.start();
	}, "group=" + groupName);

	SettingsManager::getInstance()->getParams()->addButton("Stress Test Long Drag", [&] {
		SettingsManager::getInstance()->mDrawTouches = true;
		mSimulatedTouchDriver.setBounds(Rectf(vec2(0), getWindowSize()));
		mSimulatedTouchDriver.setMinTouchDuration(4.f);
		mSimulatedTouchDriver.setMaxTouchDuration(8.f);
		mSimulatedTouchDriver.setMaxDragDistance(200.f);
		mSimulatedTouchDriver.start();
	}, "group=" + groupName);

	SettingsManager::getInstance()->getParams()->addButton("Stress Test Tap Only", [&] {
		SettingsManager::getInstance()->mDrawTouches = true;
		mSimulatedTouchDriver.setBounds(Rectf(vec2(0), getWindowSize()));
		mSimulatedTouchDriver.setMinTouchDuration(0);
		mSimulatedTouchDriver.setMaxTouchDuration(0.1f);
		mSimulatedTouchDriver.setMaxDragDistance(0.f);
		mSimulatedTouchDriver.start();
	}, "group=" + groupName);

}

}
}

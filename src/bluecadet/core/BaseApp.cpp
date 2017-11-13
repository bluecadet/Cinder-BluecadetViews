#include "BaseApp.h"
#include "SettingsManager.h"
#include "ScreenLayout.h"
#include "ScreenCamera.h"

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
	mDebugUiPadding(16.0f),
	mRootView(new BaseView()),
	mMiniMap(new MiniMapView(0.025f)),
	mStats(new GraphView(ivec2(128, 48))),
	mIsLateSetupCompleted(false)
{
}

BaseApp::~BaseApp() {
}

void BaseApp::setup() {
	auto settings = SettingsManager::getInstance();
	
	if (settings->mDisplaySize.x <= 0) settings->mDisplaySize.x = getWindowWidth();
	if (settings->mDisplaySize.y <= 0) settings->mDisplaySize.y = getWindowHeight();

	ScreenLayout::getInstance()->getAppSizeChangedSignal().connect(bind(&BaseApp::handleAppSizeChange, this, placeholders::_1));
	ScreenLayout::getInstance()->setup(settings->mDisplaySize, settings->mDisplayRows, settings->mDisplayColumns);

	ScreenCamera::getInstance()->setup(ScreenLayout::getInstance());
	ScreenCamera::getInstance()->getViewportChangedSignal().connect(bind(&BaseApp::handleViewportChange, this, placeholders::_1));
	ScreenCamera::getInstance()->setZoomToggleHotkeyEnabled(settings->mZoomToggleHotkeyEnabled);
	ScreenCamera::getInstance()->setDisplayIdHotkeysEnabled(settings->mDisplayIdHotkeysEnabled);
	ScreenCamera::getInstance()->zoomToFitWindow();

	if (settings->mCameraOffset != vec2(0)) {
		vec2 globalOffset = settings->mCameraOffset;
		vec2 localOffset = globalOffset * ScreenCamera::getInstance()->getScale();
		vec2 currentOffset = ScreenCamera::getInstance()->getTranslation();
		ScreenCamera::getInstance()->setTranslation(currentOffset + localOffset);
	}

	// Apply run-time settings
	if (settings->mShowCursor) {
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
	if (settings->mMouseEnabled) {
		mMouseDriver.connect();
		mMouseDriver.setVirtualMultiTouchEnabled(true);
	}
	if (settings->mTuioTouchEnabled) {
		mTuioDriver.connect();
	}
	if (settings->mNativeTouchEnabled) {
		mNativeTouchDriver.connect();
	}

	mSimulatedTouchDriver.setup(Rectf(vec2(0), getWindowSize()), 60);

	// Debugging
	const float targetFps = (float)settings->mFps;
	mStats->setBackgroundColor(ColorA(0, 0, 0, 0.1f));
	mStats->addGraph("FPS", 0, targetFps, ColorA(1.0f, 0.0f, 0.0f, 0.75f), ColorA(0.0f, 1.0f, 0.25f, 0.75f));
}

void BaseApp::update() {
	if (!mIsLateSetupCompleted && getElapsedFrames() > 1) {
		lateSetup();
		mIsLateSetupCompleted = true;
	}

	const double currentTime = getElapsedSeconds();
	const double deltaTime = mLastUpdateTime == 0 ? 0 : currentTime - mLastUpdateTime;
	mLastUpdateTime = currentTime;

	// get the screen layout's transform and apply it to all
	// touch events to convert touches from window into app space
	const auto appTransform = glm::inverse(ScreenCamera::getInstance()->getTransform());
	const auto appSize = ScreenLayout::getInstance()->getAppSize();
	touch::TouchManager::getInstance()->update(mRootView, appSize, appTransform);
	mRootView->updateScene(deltaTime);

	mStats->addValue("FPS", 1.0f / (float)deltaTime);
}

void BaseApp::draw(const bool clear) {
	auto settings = SettingsManager::getInstance();

	if (clear) {
		gl::clear(settings->mClearColor);
	}

	{
		gl::ScopedModelMatrix scopedMatrix;
		// apply screen layout transform to root view
		gl::multModelMatrix(ScreenCamera::getInstance()->getTransform());
		mRootView->drawScene();

		// draw debug touches in app coordinate space
		if (settings->mDebugEnabled && settings->mShowTouches) {
			touch::TouchManager::getInstance()->debugDrawTouches();
		}
	}

	if (settings->mDebugEnabled) {
		// draw params and debug layers in window coordinate space
		if (settings->mShowScreenLayout) {
			gl::ScopedModelMatrix scopedMatrix;
			gl::multModelMatrix(ScreenCamera::getInstance()->getTransform());
			ScreenLayout::getInstance()->draw();
		}
		if (settings->mShowMinimap) {
			mMiniMap->drawScene();
		}
		if (settings->mShowStats) {
			mStats->drawScene();
		}
		if (settings->getParams()->isVisible()) {
			settings->getParams()->draw();
		}
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
	case KeyEvent::KEY_c:
		SettingsManager::getInstance()->mShowCursor = !SettingsManager::getInstance()->mShowCursor;
		SettingsManager::getInstance()->mShowCursor ? showCursor() : hideCursor();
		break;
	case KeyEvent::KEY_f:
		SettingsManager::getInstance()->mFullscreen = !isFullScreen();
		setFullScreen(SettingsManager::getInstance()->mFullscreen);
		ScreenCamera::getInstance()->zoomToFitWindow();
		break;
	case KeyEvent::KEY_F1:
		if (!SettingsManager::getInstance()->getParams()->isVisible()) {
			SettingsManager::getInstance()->getParams()->show();
			SettingsManager::getInstance()->getParams()->maximize();

		} else if (SettingsManager::getInstance()->getParams()->isMaximized()) {
			if (event.isShiftDown()) {
				SettingsManager::getInstance()->getParams()->hide();
			} else {
				SettingsManager::getInstance()->getParams()->minimize();
			}

		} else {
			SettingsManager::getInstance()->getParams()->show();
			SettingsManager::getInstance()->getParams()->maximize();
		}
		break;
	}
}

void BaseApp::handleAppSizeChange(const ci::ivec2 & appSize) {
	mRootView->setSize(vec2(appSize));
	mMiniMap->setLayout(
		ScreenLayout::getInstance()->getNumColumns(),
		ScreenLayout::getInstance()->getNumRows(),
		ScreenLayout::getInstance()->getDisplaySize()
	);
}

void BaseApp::handleViewportChange(const ci::Area & viewport) {
	mMiniMap->setViewport(viewport);
	mMiniMap->setPosition(vec2(getWindowSize()) - mMiniMap->getSize() - vec2(mDebugUiPadding));
	mStats->setPosition(vec2(mDebugUiPadding, (float)getWindowHeight() - mStats->getHeight() - mDebugUiPadding));
	SettingsManager::getInstance()->getParams()->setPosition(vec2(mDebugUiPadding));
}

void BaseApp::addTouchSimulatorParams(float touchesPerSecond) {

	mSimulatedTouchDriver.setTouchesPerSecond(touchesPerSecond);

	const string groupName = "Touch Sim";
	auto params = SettingsManager::getInstance()->getParams();

	params->addParam<bool>("Enabled", [&](bool v) {
		if (!mSimulatedTouchDriver.isRunning()) {
			SettingsManager::getInstance()->mShowTouches = true;
			mSimulatedTouchDriver.setBounds(Rectf(vec2(0), getWindowSize()));
			mSimulatedTouchDriver.start();
		} else {
			SettingsManager::getInstance()->mShowTouches = false;
			mSimulatedTouchDriver.stop();
		}
	}, [&] {
		return SettingsManager::getInstance()->mShowTouches && mSimulatedTouchDriver.isRunning();
	}).group(groupName);

	static int stressTestMode = 0;
	static vector<string> stressTestModes = {"Tap & Drag", "Slow Drag", "Tap"};

	params->addParam("Mode", stressTestModes, &stressTestMode).updateFn([&] {
		if (stressTestMode == 0) {
			mSimulatedTouchDriver.setMinTouchDuration(0);
			mSimulatedTouchDriver.setMaxTouchDuration(1.f);
			mSimulatedTouchDriver.setMaxDragDistance(200.f);
		} else if (stressTestMode == 1) {
			mSimulatedTouchDriver.setMinTouchDuration(4.f);
			mSimulatedTouchDriver.setMaxTouchDuration(8.f);
			mSimulatedTouchDriver.setMaxDragDistance(200.f);
		} else if (stressTestMode == 2) {
			mSimulatedTouchDriver.setMinTouchDuration(0);
			mSimulatedTouchDriver.setMaxTouchDuration(0.1f);
			mSimulatedTouchDriver.setMaxDragDistance(0.f);
		}
	}).group(groupName);

	params->addParam<float>("Touches/s", [&](float v) {
		mSimulatedTouchDriver.setTouchesPerSecond(v);
	}, [&]() {
		return mSimulatedTouchDriver.getTouchesPerSecond();
	}).group(groupName);

	params->addParam<bool>("Show Missed Touches", [&](bool v) {
		TouchManager::getInstance()->setDiscardMissedTouches(!v);
	}, [&]() {
		return !TouchManager::getInstance()->getDiscardMissedTouches();
	}).group(groupName);

	if (SettingsManager::getInstance()->mCollapseParams) {
		params->setOptions(groupName, "opened=false");
	}
}

}
}
#include "BaseApp.h"
#include "ScreenCamera.h"
#include "ScreenLayout.h"
#include "SettingsManager.h"

// Needed for SetForegroundWindow
#if defined(CINDER_MSW)
#include <Windows.h>
#endif

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::views;

#ifndef NO_TOUCH
using namespace bluecadet::touch;
using namespace bluecadet::touch::drivers;
#endif

namespace bluecadet {
namespace core {

BaseApp::BaseApp()
	: ci::app::App(),
	  mLastUpdateTime(0),
	  mDebugUiPadding(16.0f),
	  mRootView(new BaseView()),
	  mMiniMap(new MiniMapView(0.025f)),
	  mStats(new GraphView(ivec2(128, 48))),
	  mIsLateSetupCompleted(false) {}

BaseApp::~BaseApp() {}

void BaseApp::setup() {
	SettingsManager::getInstance()->getSignalSettingsLoaded().connect(bind(&BaseApp::handleSettingsLoaded, this));
	ScreenLayout::getInstance()->getAppSizeChangedSignal().connect(
		bind(&BaseApp::handleAppSizeChange, this, placeholders::_1));
	ScreenCamera::getInstance()->getViewportChangedSignal().connect(
		bind(&BaseApp::handleViewportChange, this, placeholders::_1));

	handleSettingsLoaded();

#ifndef NO_TOUCH
	mSimulatedTouchDriver.setup(Rectf(vec2(0), getWindowSize()), 60);
#endif

#if defined(CINDER_MSW)
	// Move window to foreground so that console output doesn't obstruct it
	HWND nativeWindow = (HWND)getWindow()->getNative();
	::SetForegroundWindow(nativeWindow);
#endif

	// Debugging
	mStats->setBackgroundColor(ColorA(0, 0, 0, 0.1f));
	mStats->addGraph("FPS", 0, getFrameRate(), ColorA(1.0f, 0.0f, 0.0f, 0.75f), ColorA(0.0f, 1.0f, 0.25f, 0.75f));

	if (SettingsManager::getInstance()->mTouchSimEnabled) {
		addTouchSimulatorParams(SettingsManager::getInstance()->mSimulatedTouchesPerSecond);
	}
}

void BaseApp::update() {
	if (!mIsLateSetupCompleted && getElapsedFrames() > 1) {
		lateSetup();
		mIsLateSetupCompleted = true;
	}

	
	const double absoluteTime = getElapsedSeconds();
	const double deltaTime = mLastUpdateTime == 0 ? 0 : absoluteTime - mLastUpdateTime;

	BaseView::FrameInfo frameInfo(absoluteTime, deltaTime);
	mLastUpdateTime = absoluteTime;

	// get the screen layout's transform and apply it to all
	// touch events to convert touches from window into app space
	const auto appTransform = glm::inverse(ScreenCamera::getInstance()->getTransform());
	const auto appSize = ScreenLayout::getInstance()->getAppSize();

#ifndef NO_TOUCH
	touch::TouchManager::getInstance()->update(mRootView, appSize, appTransform);
#endif

	mRootView->updateScene(frameInfo);

	mStats->addValue("FPS", 1.0f / (float)frameInfo.deltaTime);
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

#ifndef NO_TOUCH
		// draw debug touches in app coordinate space
		if (settings->mDebugEnabled && settings->mShowTouches) {
			touch::TouchManager::getInstance()->debugDrawTouches();
		}
#endif
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
		case KeyEvent::KEY_q: quit(); break;
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

void BaseApp::findAssetDir(const std::string & subPath, bool stopAtFirst) {
	fs::path basePath = getAppPath();
	
	while (basePath.has_parent_path()) {
		basePath = basePath.parent_path();

		auto possiblePath = basePath;
		possiblePath = possiblePath.append(subPath);

		if (fs::exists(possiblePath)) {
			CI_LOG_I("Adding asset dir '" << possiblePath << "'");
			addAssetDirectory(possiblePath);

			if (stopAtFirst) {
				return;
			}
		}
	}
}

void BaseApp::handleAppSizeChange(const ci::ivec2 & appSize) {
	mRootView->setSize(vec2(appSize));
	mMiniMap->setLayout(ScreenLayout::getInstance()->getNumColumns(), ScreenLayout::getInstance()->getNumRows(),
						ScreenLayout::getInstance()->getDisplaySize(), ScreenLayout::getInstance()->getBezelDims());
}

void BaseApp::handleViewportChange(const ci::Area & viewport) {
	mMiniMap->setViewport(viewport);
	mMiniMap->setPosition(vec2(getWindowSize()) - mMiniMap->getSize() - vec2(mDebugUiPadding));
	mStats->setPosition(vec2(mDebugUiPadding, (float)getWindowHeight() - mStats->getHeight() - mDebugUiPadding));
	SettingsManager::getInstance()->getParams()->setPosition(vec2(mDebugUiPadding));
}
void BaseApp::handleSettingsLoaded() {
	auto settings = SettingsManager::getInstance();
	auto layout = ScreenLayout::getInstance();
	auto camera = ScreenCamera::getInstance();

	if (settings->mDisplaySize.x <= 0) settings->mDisplaySize.x = getWindowWidth();
	if (settings->mDisplaySize.y <= 0) settings->mDisplaySize.y = getWindowHeight();

	layout->setup(settings->mDisplaySize, settings->mDisplayRows, settings->mDisplayColumns, settings->mBezelDims);

	camera->setup(layout);
	camera->setZoomToggleHotkeyEnabled(settings->mZoomToggleHotkeyEnabled);
	camera->setDisplayIdHotkeysEnabled(settings->mDisplayIdHotkeysEnabled);
	camera->zoomToFitWindow();

	if (settings->mCameraOffset != vec2(0)) {
		vec2 globalOffset = settings->mCameraOffset;
		vec2 localOffset = globalOffset * camera->getScale();
		vec2 currentOffset = camera->getTranslation();
		camera->setTranslation(currentOffset + localOffset);
	}

	if (settings->mCameraZoom != 1.0f) {
		float zoom = camera->getScale().x;
		camera->zoomAtCurrentLocation(zoom * settings->mCameraZoom);
	}

	// Apply run-time settings
	if (settings->mShowCursor) {
		showCursor();
	} else {
		hideCursor();
	}

	// Set up graphics
	gl::enableVerticalSync(settings->mVerticalSync);
	gl::enableAlphaBlending();

#ifndef NO_TOUCH
	// Set up touches
	if (settings->mMouseEnabled) {
		mMouseDriver.connect();
		mMouseDriver.setVirtualMultiTouchEnabled(true);
	} else {
		mMouseDriver.disconnect();
	}
	if (settings->mTuioTouchEnabled) {
		mTuioDriver.connect();
	} else {
		mTuioDriver.disconnect();
	}
	if (settings->mNativeTouchEnabled) {
		if (settings->mSupportMultipleNativeTouchScreens) {
			mMultiNativeTouchDriver.connect();
		} else {
			mNativeTouchDriver.connect();
		}
	} else {
		mMultiNativeTouchDriver.disconnect();
		mNativeTouchDriver.disconnect();
	}
#endif  // !NO_TOUCH
}


#ifndef NO_TOUCH
void BaseApp::addTouchSimulatorParams(float touchesPerSecond) {

	mSimulatedTouchDriver.setTouchesPerSecond(touchesPerSecond);

	const string groupName = "Touch Sim";
	auto params = SettingsManager::getInstance()->getParams();

	params
		->addParam<bool>(
			"Enabled",
			[&](bool v) {
				if (!mSimulatedTouchDriver.isRunning()) {
					SettingsManager::getInstance()->mShowTouches = true;
					mSimulatedTouchDriver.setBounds(Rectf(vec2(0), getWindowSize()));
					mSimulatedTouchDriver.start();
				} else {
					SettingsManager::getInstance()->mShowTouches = false;
					mSimulatedTouchDriver.stop();
				}
			},
			[&] { return SettingsManager::getInstance()->mShowTouches && mSimulatedTouchDriver.isRunning(); })
		.group(groupName);

	static int stressTestMode = 0;
	static vector<string> stressTestModes = {"Tap & Drag", "Slow Drag", "Tap"};

	params->addParam("Mode", stressTestModes, &stressTestMode)
		.updateFn([&] {
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
		})
		.group(groupName);

	params
		->addParam<float>("Touches/s", [&](float v) { mSimulatedTouchDriver.setTouchesPerSecond(v); },
						  [&]() { return mSimulatedTouchDriver.getTouchesPerSecond(); })
		.group(groupName);

	params
		->addParam<bool>("Show Missed Touches",
						 [&](bool v) { TouchManager::getInstance()->setDiscardMissedTouches(!v); },
						 [&]() { return !TouchManager::getInstance()->getDiscardMissedTouches(); })
		.group(groupName);

	if (SettingsManager::getInstance()->mCollapseParams) {
		params->setOptions(groupName, "opened=false");
	}
}
#endif

}  // namespace core
}  // namespace bluecadet
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

//! Prints all messages to std::cout/std::cerr instead of ci::app::console()
class StdOutLogger : public ci::log::Logger {
public:
	StdOutLogger(bool timestampEnabled = true) : Logger() { setTimestampEnabled(timestampEnabled); };
	void write(const ci::log::Metadata & meta, const std::string & text) override {
		std::ostream * stream = &std::cout;
		switch (meta.mLevel) {
		case ci::log::LEVEL_ERROR:
		case ci::log::LEVEL_FATAL: {
			stream = &std::cerr;
			break;
		}
		default:
			break;
		}
		writeDefault((*stream), meta, text);
	};
};

namespace bluecadet {
	namespace core {

		BaseApp::BaseApp()
			: ci::app::App(),
			mLastUpdateTime(0),
			mDebugUiPadding(16.0f),
			mRootView(new BaseView()),
			mMiniMap(new MiniMapView(0.025f)),
			mStats(new GraphView(ivec2(128, 48))),
			mIsLateSetupCompleted(false) {
		}

		BaseApp::~BaseApp() {
		}

		void BaseApp::setup() {
			SettingsManager::get()->getSignalSettingsLoaded().connect(bind(&BaseApp::handleSettingsLoaded, this));
			ScreenLayout::get()->getAppSizeChangedSignal().connect(bind(&BaseApp::handleAppSizeChange, this, placeholders::_1));
			ScreenCamera::get()->getViewportChangedSignal().connect(bind(&BaseApp::handleViewportChange, this, placeholders::_1));

			handleSettingsLoaded();

#ifndef NO_TOUCH
			mSimulatedTouchDriver.setup(Rectf(vec2(0), getWindowSize()), 60);
#endif

#if defined(CINDER_MSW)
			HWND nativeWindow = (HWND)getWindow()->getNative();
			// This ensures that this app can be run in windowed mode when launched from PM2
			::ShowWindow(nativeWindow, SW_SHOW);
			// Move window to foreground so that console output doesn't obstruct it
			::SetForegroundWindow(nativeWindow);
			::SetFocus(nativeWindow);
#endif

			// Debugging
			mStats->setBackgroundColor(ColorA(0, 0, 0, 0.1f));
			mStats->addGraph("FPS", 0, getFrameRate(), ColorA(0, 1.0f, 0, 1.0f));

			if (SettingsManager::get()->mTouchSimEnabled) {
				addTouchSimulatorParams(SettingsManager::get()->mSimulatedTouchesPerSecond);
			}

			if (SettingsManager::get()->mLogToStdOut) {
				ci::log::LogManager::instance()->addLogger(make_shared<StdOutLogger>());
				if (SettingsManager::get()->mConsole) {
					CI_LOG_W("Logging to console and to stdout at the same time is not supported. You can only set one of these to true. Console will override stdout.");
				}
			}
		}

		void BaseApp::update() {
			if (!mIsLateSetupCompleted && getElapsedFrames() > 1) {
				lateSetup();
				mIsLateSetupCompleted = true;
			}

			const double absoluteTime = getElapsedSeconds();
			const double deltaTime    = mLastUpdateTime == 0 ? 0 : absoluteTime - mLastUpdateTime;

			BaseView::FrameInfo frameInfo(absoluteTime, deltaTime);
			mLastUpdateTime = absoluteTime;

			// get the screen layout's transform and apply it to all
			// touch events to convert touches from window into app space
			const auto appTransform = glm::inverse(ScreenCamera::get()->getTransform());
			const auto appSize      = ScreenLayout::get()->getAppSize();

#ifndef NO_TOUCH
			touch::TouchManager::get()->update(mRootView, appSize, appTransform);
#endif

			mRootView->updateScene(frameInfo);

			mStats->addValue("FPS", 1.0f / (float)frameInfo.deltaTime);
		}

		void BaseApp::draw(const bool clear) {
			auto settings = SettingsManager::get();

			if (clear) {
				gl::clear(settings->mClearColor);
			}

			{
				gl::ScopedModelMatrix scopedMatrix;
				// apply screen layout transform to root view
				gl::multModelMatrix(ScreenCamera::get()->getTransform());
				mRootView->drawScene();

#ifndef NO_TOUCH
				// draw debug touches in app coordinate space
				if (settings->mDebugEnabled && settings->mShowTouches) {
					touch::TouchManager::get()->debugDrawTouches();
				}
#endif
			}

			if (settings->mDebugEnabled) {
				// draw params and debug layers in window coordinate space
				if (settings->mShowScreenLayout) {
					gl::ScopedModelMatrix scopedMatrix;
					gl::multModelMatrix(ScreenCamera::get()->getTransform());
					ScreenLayout::get()->draw();
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
				SettingsManager::get()->mShowCursor = !SettingsManager::get()->mShowCursor;
				SettingsManager::get()->mShowCursor ? showCursor() : hideCursor();
				break;
			case KeyEvent::KEY_f:
				SettingsManager::get()->mFullscreen = !isFullScreen();
				setFullScreen(SettingsManager::get()->mFullscreen);
				ScreenCamera::get()->zoomToFitWindow();
				break;
			case KeyEvent::KEY_F1:
				if (!SettingsManager::get()->getParams()->isVisible()) {
					SettingsManager::get()->getParams()->show();
					SettingsManager::get()->getParams()->maximize();

				} else if (SettingsManager::get()->getParams()->isMaximized()) {
					if (event.isShiftDown()) {
						SettingsManager::get()->getParams()->hide();
					} else {
						SettingsManager::get()->getParams()->minimize();
					}

				} else {
					SettingsManager::get()->getParams()->show();
					SettingsManager::get()->getParams()->maximize();
				}
				break;
			}
		}

		void BaseApp::findAssetDir(const std::string & subPath, bool stopAtFirst) {
			fs::path basePath = getAppPath();

			while (basePath.has_parent_path()) {
				basePath = basePath.parent_path();

				auto possiblePath = basePath;
				possiblePath      = possiblePath.append(subPath);

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
			mMiniMap->setLayout(ScreenLayout::get()->getNumColumns(), ScreenLayout::get()->getNumRows(),
				ScreenLayout::get()->getDisplaySize(), ScreenLayout::get()->getBezelDims());
		}

		void BaseApp::handleViewportChange(const ci::Area & viewport) {
			mMiniMap->setViewport(viewport);
			mMiniMap->setPosition(vec2(getWindowSize()) - mMiniMap->getSize() - vec2(mDebugUiPadding));
			mStats->setPosition(vec2(mDebugUiPadding, (float)getWindowHeight() - mStats->getHeight() - mDebugUiPadding));
			SettingsManager::get()->getParams()->setPosition(vec2(mDebugUiPadding));
		}
		void BaseApp::handleSettingsLoaded() {
			auto settings = SettingsManager::get();
			auto layout   = ScreenLayout::get();
			auto camera   = ScreenCamera::get();

			if (settings->mDisplaySize.x <= 0) settings->mDisplaySize.x = getWindowWidth();
			if (settings->mDisplaySize.y <= 0) settings->mDisplaySize.y = getWindowHeight();

			layout->setup(settings->mDisplaySize, settings->mDisplayRows, settings->mDisplayColumns, settings->mBezelDims);

			camera->setup(layout);
			camera->setZoomToggleHotkeyEnabled(settings->mZoomToggleHotkeyEnabled);
			camera->setDisplayIdHotkeysEnabled(settings->mDisplayIdHotkeysEnabled);
			camera->zoomToFitWindow();

			if (settings->mCameraOffset != vec2(0)) {
				vec2 globalOffset  = settings->mCameraOffset;
				vec2 localOffset   = globalOffset * camera->getScale();
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
			auto params            = SettingsManager::get()->getParams();

			params
				->addParam<bool>(
					"Enabled",
					[&](bool v) {
						if (!mSimulatedTouchDriver.isRunning()) {
							mSimulatedTouchDriver.setBounds(Rectf(vec2(0), getWindowSize()));
							mSimulatedTouchDriver.start();
						} else {
							mSimulatedTouchDriver.stop();
						}
					},
					[&] { return SettingsManager::get()->mShowTouches && mSimulatedTouchDriver.isRunning(); })
				.group(groupName)
						.key("d");

					static int stressTestMode             = 0;
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
								->addParam<float>(
									"Touches/s", [&](float v) { mSimulatedTouchDriver.setTouchesPerSecond(v); },
									[&]() { return mSimulatedTouchDriver.getTouchesPerSecond(); })
								.group(groupName);

							params
								->addParam<bool>(
									"Show Missed Touches", [&](bool v) { TouchManager::get()->setDiscardMissedTouches(!v); },
									[&]() { return !TouchManager::get()->getDiscardMissedTouches(); })
								.group(groupName);

							if (SettingsManager::get()->mCollapseParams) {
								params->setOptions(groupName, "opened=false");
							}
		}
#endif

	}  // namespace core
}  // namespace bluecadet

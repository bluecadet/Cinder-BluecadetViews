#include "BaseApp.h"
#include "SettingsManager.h"
#include "ScreenLayout.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::utils;
using namespace bluecadet::touch;
using namespace bluecadet::touch::drivers;

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

void BaseApp::prepareSettings(ci::app::App::Settings *settings) {
	fs::path appSettingsPath = ci::app::getAssetPath("appSettings.json");
	SettingsManager::getInstance()->setup(appSettingsPath, settings);
}


void BaseApp::setup() {
	auto settings = SettingsManager::getInstance();

	// Set up screen layout
	int displayWidth = settings->hasField("settings.display.width") ? settings->getField<int>("settings.display.width") : ScreenLayout::getInstance()->getDisplayWidth();
	int displayHeight = settings->hasField("settings.display.width") ? settings->getField<int>("settings.display.height") : ScreenLayout::getInstance()->getDisplayHeight();
	int rows = settings->hasField("settings.display.rows") ? settings->getField<int>("settings.display.rows") : ScreenLayout::getInstance()->getNumRows();
	int cols = settings->hasField("settings.display.columns") ? settings->getField<int>("settings.display.columns") : ScreenLayout::getInstance()->getNumColumns();
	
	ScreenLayout::getInstance()->setup(mRootView, ivec2(displayWidth, displayHeight), rows, cols);
	ScreenLayout::getInstance()->zoomToFitWindow();
	
	// Apply settings
	if (settings->mShowMouse) {
		showCursor();
	} else {
		hideCursor();
	}

	// Set up graphics
	gl::enableVerticalSync(settings->mVerticalSync);
	gl::enableAlphaBlending();

	// Set up touches
	mMouseDriver.connect();
	mTuioDriver.connect();
	mSimulatedTouchDriver.setup(Rectf(vec2(0), getWindowSize()), 60);
}

void BaseApp::update() {
	const double currentTime = getElapsedSeconds();
	const double deltaTime = mLastFrameTime == 0 ? 0 : currentTime - mLastFrameTime;
	mLastFrameTime = currentTime;

	touch::TouchManager::getInstance()->update(mRootView);
	mRootView->updateScene(deltaTime);
}

void BaseApp::draw() {
	auto settings = SettingsManager::getInstance();

	gl::clear(settings->mClearColor);

	mRootView->drawScene();

	if (settings->mDebugMode) {
		if (settings->mDebugDrawScreenLayout) {
			ScreenLayout::getInstance()->draw();
		}

		settings->getParams()->draw();

		if (settings->mDebugDrawTouches) {
			touch::TouchManager::getInstance()->debugDrawTouches();
		}
	}
}

void BaseApp::keyDown(KeyEvent event) {
	switch (event.getCode()) {
		case KeyEvent::KEY_q:
			quit();
			break;
	}
}

void BaseApp::addTouchSimulatorParams(float touchesPerSecond) {

	mSimulatedTouchDriver.setTouchesPerSecond(touchesPerSecond);

	SettingsManager::getInstance()->getParams()->addSeparator();
	SettingsManager::getInstance()->getParams()->addText("Touch Simulator");
	SettingsManager::getInstance()->getParams()->addParam<float>("Touches/s", [&](float v) { mSimulatedTouchDriver.setTouchesPerSecond(v); }, [&]() { return mSimulatedTouchDriver.getTouchesPerSecond(); });
	SettingsManager::getInstance()->getParams()->addParam<bool>("Stress Test",
		[&](bool v) { SettingsManager::getInstance()->mDebugDrawTouches = v; v ? mSimulatedTouchDriver.start() : mSimulatedTouchDriver.stop(); },
		[&] { return SettingsManager::getInstance()->mDebugDrawTouches && mSimulatedTouchDriver.isRunning(); });
	SettingsManager::getInstance()->getParams()->addButton("Stress Test Tap + Drag", [&] {
		SettingsManager::getInstance()->mDebugDrawTouches = true;
		mSimulatedTouchDriver.setMinTouchDuration(0);
		mSimulatedTouchDriver.setMaxTouchDuration(1.f);
		mSimulatedTouchDriver.setMaxDragDistance(200.f);
		mSimulatedTouchDriver.start();
	});
	SettingsManager::getInstance()->getParams()->addButton("Stress Test Long Drag", [&] {
		SettingsManager::getInstance()->mDebugDrawTouches = true;
		mSimulatedTouchDriver.setMinTouchDuration(4.f);
		mSimulatedTouchDriver.setMaxTouchDuration(8.f);
		mSimulatedTouchDriver.setMaxDragDistance(200.f);
		mSimulatedTouchDriver.start();
	});
	SettingsManager::getInstance()->getParams()->addButton("Stress Test Tap Only", [&] {
		SettingsManager::getInstance()->mDebugDrawTouches = true;
		mSimulatedTouchDriver.setMinTouchDuration(0);
		mSimulatedTouchDriver.setMaxTouchDuration(0.1f);
		mSimulatedTouchDriver.setMaxDragDistance(0.f);
		mSimulatedTouchDriver.start();
	});

}

}
}

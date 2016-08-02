#include "BaseApp.h"
#include "SettingsManager.h"
#include "ScreenLayoutView.h"

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
	int width = SettingsManager::getInstance()->getField<int>("settings.display.width");
	int height = SettingsManager::getInstance()->getField<int>("settings.display.height");
	int rows = SettingsManager::getInstance()->getField<int>("settings.display.totalRows");
	int columns = SettingsManager::getInstance()->getField<int>("settings.display.totalColumns");
	ScreenLayoutView::getInstance()->setup(mRootView, width, height, rows, columns);
	
	mRootView->addChild(ScreenLayoutView::getInstance());

	// Set if screen layout will draw
	SettingsManager::getInstance()->mDebugDrawScreenLayout ? ScreenLayoutView::getInstance()->setAlpha(1.0f) : ScreenLayoutView::getInstance()->setAlpha(0.0f);

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

	if (SettingsManager::getInstance()->mDebugMode) {
		mParams = SettingsManager::getInstance()->getParams();
	}
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

	if (SettingsManager::getInstance()->mDebugDrawTouches) {
		touch::TouchManager::getInstance()->debugDrawTouches();
	}

	if (SettingsManager::getInstance()->mDebugMode) {
		mParams->draw();
	}
}

void BaseApp::keyDown(KeyEvent event) {
	switch (event.getCode()) {
	case KeyEvent::KEY_q:
		quit();
		break;
	}
}

void BaseApp::connectSimulatedTouchDriver(int touchesPerSecond) {
	console() << ScreenLayoutView::getInstance()->getAppWidth() << "   " << ScreenLayoutView::getInstance()->getAppHeight() << endl;
	mSimulatedTouchDriver.setup(Rectf(0.0f, 0.0f, (float)ScreenLayoutView::getInstance()->getAppWidth(), (float)ScreenLayoutView::getInstance()->getAppHeight()), (float)touchesPerSecond);

	SettingsManager::getInstance()->getParams()->addSeparator();
	SettingsManager::getInstance()->getParams()->addText("Touch Simulator");
	SettingsManager::getInstance()->getParams()->addParam<bool>("Stress Test",
		[=](bool v) { SettingsManager::getInstance()->mDebugDrawTouches = v; v ? mSimulatedTouchDriver.start() : mSimulatedTouchDriver.stop(); },
		[=] { return SettingsManager::getInstance()->mDebugDrawTouches && mSimulatedTouchDriver.isRunning(); }).key("g");
	SettingsManager::getInstance()->getParams()->addButton("Stress Test Tap + Drag", [=] {
		SettingsManager::getInstance()->mDebugDrawTouches = true;
		mSimulatedTouchDriver.setMinTouchDuration(0);
		mSimulatedTouchDriver.setMaxTouchDuration(1.f);
		mSimulatedTouchDriver.setMaxDragDistance(200.f);
		mSimulatedTouchDriver.start();
	});
	SettingsManager::getInstance()->getParams()->addButton("Stress Test Long Drag", [=] {
		SettingsManager::getInstance()->mDebugDrawTouches = true;
		mSimulatedTouchDriver.setMinTouchDuration(4.f);
		mSimulatedTouchDriver.setMaxTouchDuration(8.f);
		mSimulatedTouchDriver.setMaxDragDistance(200.f);
		mSimulatedTouchDriver.start();
	});
	SettingsManager::getInstance()->getParams()->addButton("Stress Test Tap Only", [=] {
		SettingsManager::getInstance()->mDebugDrawTouches = true;
		mSimulatedTouchDriver.setMinTouchDuration(0);
		mSimulatedTouchDriver.setMaxTouchDuration(0.1f);
		mSimulatedTouchDriver.setMaxDragDistance(0.f);
		mSimulatedTouchDriver.start();
	});

}

}
}
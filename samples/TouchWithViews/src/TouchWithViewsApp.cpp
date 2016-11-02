#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Rand.h"

#include <core/BaseApp.h>
#include <views/TouchView.h>

#include "Btn.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::core;
using namespace bluecadet::views;
using namespace bluecadet::touch;

class BaseAppSampleApp : public BaseApp {
public:
	static void prepareSettings(ci::app::App::Settings* settings);
	void setup() override;
};

void BaseAppSampleApp::prepareSettings(ci::app::App::Settings* settings) {
	SettingsManager::getInstance()->mFullscreen = false;
	SettingsManager::getInstance()->mWindowSize = ivec2(1280, 720);
	SettingsManager::getInstance()->mBorderless = false;
	BaseApp::prepareSettings(settings);
}

void BaseAppSampleApp::setup() {

	BaseApp::setup();
	BaseApp::addTouchSimulatorParams();

	// Optional: configure the size and background of your root view
	getRootView()->setBackgroundColor(Color::gray(0.5f));
	getRootView()->setSize(ScreenLayout::getInstance()->getAppSize());
	
	// Create a buttons using the Btn class
	for (int i = 0; i < 10; ++i) {
		auto btn = BtnRef(new Btn());
		btn->setup();
		btn->setDebugDrawTouchPath(true);
		btn->setPosition(vec2(randFloat(getRootView()->getWidth()), randFloat(getRootView()->getHeight())));
		btn->setTint(Color::hex(randInt(0xffffff)));

		btn->mDidBeginTouch.connect([=](const bluecadet::touch::TouchEvent& event) { event.target->setScale(2.0f); });
		btn->mDidEndTouch.connect([=] (const bluecadet::touch::TouchEvent& event) { event.target->setScale(1.0f); });

		getRootView()->addChild(btn);
	}
}

// Make sure to pass a reference to prepareSettings to configure the app correctly. MSAA and other render options are optional.
CINDER_APP(BaseAppSampleApp, RendererGl(RendererGl::Options().msaa(4)), BaseAppSampleApp::prepareSettings);

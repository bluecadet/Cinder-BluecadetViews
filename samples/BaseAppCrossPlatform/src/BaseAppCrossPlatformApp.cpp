#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Rand.h"

#include "bluecadet/core/BaseApp.h"
#include "bluecadet/views/TouchView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::core;
using namespace bluecadet::views;
using namespace bluecadet::touch;

class BaseAppCrossPlatformApp : public BaseApp {
public:
	static void prepareSettings(ci::app::App::Settings* settings);
	void setup() override;
	void update() override;
	void draw() override;
};

void BaseAppCrossPlatformApp::prepareSettings(ci::app::App::Settings* settings) {
	// Initialize the settings manager with the cinder app settings and the settings json
	SettingsManager::getInstance()->setup(settings, ci::app::getAssetPath("appSettings.json"), [](SettingsManager * manager) {
		// Optional: Override json defaults at runtime
		manager->mFullscreen = false;
		manager->mWindowSize = ivec2(720, 405);
		manager->mConsoleWindowEnabled = false;
		manager->mDrawMinimap = true;
		manager->mDrawStats = true;
		manager->mDrawTouches = true;
	});
}

void BaseAppCrossPlatformApp::setup() {
	BaseApp::setup();
	BaseApp::addTouchSimulatorParams();

	// Optional: configure your root view
	getRootView()->setBackgroundColor(Color::gray(0.5f));

	// Sample content
	auto button = TouchViewRef(new TouchView());
	button->setPosition(vec2(400, 300));
	button->setSize(vec2(200, 100));
	button->setBackgroundColor(Color(1, 0, 0));
	button->getSignalTapped().connect([=](bluecadet::touch::TouchEvent e) {
		ColorAf color = hsvToRgb(vec3(randFloat(), 1.0f, 1.0f));
		button->getTimeline()->apply(&button->getBackgroundColor(), color, 0.33f, easeInOutQuad);
	});
	
	getRootView()->addChild(button);
}

void BaseAppCrossPlatformApp::update() {
	// Optional override. BaseApp::update() will update all views.
	BaseApp::update();
}

void BaseAppCrossPlatformApp::draw() {
	// Optional override. BaseApp::draw() will draw all views.
	BaseApp::draw();
}

// Make sure to pass a reference to prepareSettings to configure the app correctly. MSAA and other render options are optional.
CINDER_APP(BaseAppCrossPlatformApp, RendererGl(RendererGl::Options().msaa(4)), BaseAppCrossPlatformApp::prepareSettings);
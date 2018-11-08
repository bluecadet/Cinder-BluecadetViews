// Cinder
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

// Bluecadet
#include "bluecadet/core/BaseApp.h"
#include "bluecadet/views/TouchView.h"

using namespace std;
using namespace ci;
using namespace ci::app;
using namespace bluecadet::core;
using namespace bluecadet::views;
using namespace bluecadet::touch;

class _TBOX_PREFIX_App : public BaseApp {
public:
	static void prepareSettings(ci::app::App::Settings *settings);
	void setup() override;
	void update() override;
	void draw() override;
};

void _TBOX_PREFIX_App::prepareSettings(ci::app::App::Settings *settings) {
	// Optional: Override the shared settings manager instance with your subclass
	// SettingsManager::setInstance(myApp::MyAppSettingsManager::getInstance());

	// Initialize the settings manager with the cinder app settings and the settings json
	SettingsManager::getInstance()->setup(
		settings,
		ci::app::getAssetPath("settings.json"),
		[](SettingsManager *manager) {
			// Optional: Override json defaults at runtime
			manager->mFullscreen = false;
			manager->mWindowSize = ivec2(1280, 720);
		});
}

void _TBOX_PREFIX_App::setup() {
	BaseApp::setup();
	
	// Optional: configure your root view
	getRootView()->setBackgroundColor(Color::gray(0.5f));

	// Sample content
	auto button = make_shared<TouchView>();
	button->setPosition(vec2(400, 300));
	button->setSize(vec2(200, 100));
	button->setBackgroundColor(Color(1, 0, 0));
	button->getSignalTapped().connect(
		[=](bluecadet::touch::TouchEvent e) { CI_LOG_I("Button tapped"); });
	getRootView()->addChild(button);
}

void _TBOX_PREFIX_App::update() {
	// Optional override. BaseApp::update() will update all views.
	BaseApp::update();
}

void _TBOX_PREFIX_App::draw() {
	// Optional override. BaseApp::draw() will draw all views.
	BaseApp::draw();
}

// Make sure to pass a reference to prepareSettings to configure the app correctly. MSAA and other render options are optional.
CINDER_APP(_TBOX_PREFIX_App,
		   RendererGl(RendererGl::Options().msaa(4)),
		   _TBOX_PREFIX_App::prepareSettings);

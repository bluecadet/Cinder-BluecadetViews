#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "bluecadet/core/BaseApp.h"
#include "bluecadet/views/TouchView.h"
#include "bluecadet/_TBOX_PREFIX_Settings.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::core;
using namespace bluecadet::views;
using namespace bluecadet::touch;

class _TBOX_PREFIX_App : public BaseApp {
public:
	static void prepareSettings(ci::app::App::Settings* settings);
	void setup() override;
	void update() override;
	void draw() override;
};

void _TBOX_PREFIX_App::prepareSettings(ci::app::App::Settings* settings) {
	// Optional: Override the shared settings manager instance with your subclass
	// This will return the correct instance whenever you call SettingsManager::get()
	SettingsManager::setInstance(bluecadet::_TBOX_PREFIX_Settings::get());
	
	// Initialize the settings manager with the cinder app settings and the settings json
	SettingsManager::get()->setup(settings);
}

void _TBOX_PREFIX_App::setup() {

	BaseApp::setup();

	// Optional: configure your root view
	getRootView()->setBackgroundColor(Color::gray(0.5f));

	// Sample content
	auto button = make_shared<TouchView>();
	button->setPosition(400.f, 300.f);
	button->setSize(200.f, 100.f);
	button->setBackgroundColor(Color(1, 0, 0));
	button->getSignalTapped().connect([=](...) { CI_LOG_I("Button tapped"); });
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
CINDER_APP(_TBOX_PREFIX_App, RendererGl(RendererGl::Options().msaa(4)), _TBOX_PREFIX_App::prepareSettings);
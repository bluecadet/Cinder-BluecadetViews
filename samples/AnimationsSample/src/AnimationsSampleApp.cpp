#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "bluecadet/core/BaseApp.h"
#include "bluecadet/views/TouchView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::core;
using namespace bluecadet::views;
using namespace bluecadet::touch;

class AnimationsSampleApp : public BaseApp {
public:
	static void prepareSettings(ci::app::App::Settings* settings);
	void setup() override;
	void update() override;
	void draw() override;
	BaseViewRef mAnimatedSizeView;
};

void AnimationsSampleApp::prepareSettings(ci::app::App::Settings* settings) {
	SettingsManager::getInstance()->setup(settings, [](SettingsManager * manager) {
		manager->mFullscreen = false;
		manager->mWindowSize = ivec2(960, 540);
		manager->mConsoleWindowEnabled = false;
	});
}

void AnimationsSampleApp::setup() {
	BaseApp::setup();

	auto button = TouchViewRef(new TouchView());
	button->setPosition(vec2(400, 300));
	button->setSize(vec2(200, 100));
	button->setBackgroundColor(Color(1, 0, 0));
	button->getSignalTapped().connect([=](bluecadet::touch::TouchEvent e) { CI_LOG_I("Button tapped"); });
	getRootView()->addChild(button);
}

void AnimationsSampleApp::update() {
	// Optional override. BaseApp::update() will update all views.
	BaseApp::update();
}

void AnimationsSampleApp::draw() {
	// Optional override. BaseApp::draw() will draw all views.
	BaseApp::draw();
}

// Make sure to pass a reference to prepareSettings to configure the app correctly. MSAA and other render options are optional.
CINDER_APP(AnimationsSampleApp, RendererGl(RendererGl::Options().msaa(4)), AnimationsSampleApp::prepareSettings);
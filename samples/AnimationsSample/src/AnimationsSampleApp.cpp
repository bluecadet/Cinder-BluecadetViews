#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#include "bluecadet/core/BaseApp.h"

#include "bluecadet/views/TextView.h"
#include "bluecadet/views/TouchView.h"
#include "bluecadet/views/ImageView.h"

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
};

void AnimationsSampleApp::prepareSettings(ci::app::App::Settings* settings) {
	SettingsManager::getInstance()->setup(settings, [](SettingsManager * manager) {
		manager->mFullscreen = false;
		manager->mWindowSize = ivec2(960, 540);
		manager->mConsoleWindowEnabled = false;
		manager->mCollapseParams = true;
	});
}

void AnimationsSampleApp::setup() {
	BaseApp::setup();

	addTouchSimulatorParams();

	BaseViewRef child = nullptr;

	if (false) {
		// BaseView
		auto view = make_shared<BaseView>();
		child = view;
	}
	if (true) {
		auto view = make_shared<TextView>();
		view->setText("Lorem ipsum dolor sit amet, consectetur adipiscing elit. \
		Donec ornare mi ut nulla iaculis accumsan. Sed placerat vitae nisl at lobortis. \
		Proin facilisis augue nec sodales sagittis. Lorem ipsum dolor sit amet, consectetur \
		adipiscing elit. Donec ornare mi ut nulla iaculis accumsan. Sed placerat vitae nisl \
		at lobortis. Proin facilisis augue nec sodales sagittis.Lorem ipsum dolor sit amet, \
		consectetur adipiscing elit. Donec ornare mi ut nulla iaculis accumsan. Sed placerat \
		vitae nisl at lobortis. Proin facilisis augue nec sodales sagittis.");
		view->setTextColor(Color::white());
		child = view;
	}
	if (false) {
		// ImageView
		auto view = make_shared<ImageView>();
		auto image = loadImage(getAssetPath("cinderblock.png"));
		auto texture = gl::Texture::create(image);
		view->setTexture(texture);
		child = view;
	}

	child->setSize(vec2(300, 200));
	child->setBackgroundColor(hsvToRgb(vec3(randFloat(), 0.8f, 1.0f)));
	child->setPosition((getRootView()->getSize() - child->getSize()) * 0.5f);
	child->getTimeline()->apply(&child->getSize(), vec2(200, 300), 2.0f, easeInOutQuad).loop(true).pingPong(true);

	getRootView()->addChild(child);
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
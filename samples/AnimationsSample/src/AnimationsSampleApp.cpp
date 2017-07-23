#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#include "bluecadet/core/BaseApp.h"

#include "bluecadet/views/TextView.h"
#include "bluecadet/views/TouchView.h"
#include "bluecadet/views/ImageView.h"
#include "bluecadet/views/FboView.h"
#include "bluecadet/views/EllipseView.h"

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
		manager->mMinimizeParams = true;
		manager->mCollapseParams = true;
	});
}

void AnimationsSampleApp::setup() {
	BaseApp::setup();

	addTouchSimulatorParams();

	vector<BaseViewRef> views;

	{
		auto textView = make_shared<TextView>();
		textView->setText("Lorem ipsum dolor sit amet, consectetur adipiscing elit. \
		Donec ornare mi ut nulla iaculis accumsan. Sed placerat vitae nisl at lobortis. \
		Proin facilisis augue nec sodales sagittis. Lorem ipsum dolor sit amet, consectetur \
		adipiscing elit. Donec ornare mi ut nulla iaculis accumsan. Sed placerat vitae nisl \
		at lobortis. Proin facilisis augue nec sodales sagittis.Lorem ipsum dolor sit amet, \
		consectetur adipiscing elit. Donec ornare mi ut nulla iaculis accumsan. Sed placerat \
		vitae nisl at lobortis. Proin facilisis augue nec sodales sagittis.");
		textView->setTextColor(Color::white());
		views.push_back(textView);
	}

	{
		auto imageView = make_shared<ImageView>();
		auto image = loadImage(getAssetPath("cinderblock.png"));
		auto texture = gl::Texture::create(image);
		imageView->setTexture(texture);
		views.push_back(imageView);
	}

	{
		auto imageView = make_shared<ImageView>();
		auto image = loadImage(getAssetPath("cinderblock.png"));
		auto texture = gl::Texture::create(image);
		imageView->setTexture(texture);

		auto fboView = make_shared<FboView>();
		fboView->addChild(imageView);
		views.push_back(fboView);
	}

	{
		auto ellipseView = make_shared<EllipseView>();
		ellipseView->setSmoothness(50.0f);
		ellipseView->setTransformOrigin(vec2(150, 100));
		views.push_back(ellipseView);
	}

	float numCols = 3;
	float numRows = 3;
	float widthPerView = getWindowWidth() / numCols;
	float heightPerView = getWindowHeight() / numRows;
	float delayPerView = 0.1f;
	for (float i = 0; i < views.size(); ++i) {
		auto view = views[(int)i];
		float col = glm::floor(glm::mod(i, numCols));
		float row = glm::floor(i / numRows);

		view->setBackgroundColor(hsvToRgb(vec3(randFloat(), 0.8f, 1.0f)));
		view->setPosition(vec2(col * widthPerView, row * heightPerView) + view->getTransformOrigin());
		view->setSize(vec2(widthPerView, heightPerView));

		view->getTimeline()->apply(&view->getSize(), vec2(view->getHeight(), view->getWidth()) * 0.25f, 2.0f, easeInOutQuad)
			.loop(true).pingPong(true).delay(i / (float)views.size());
		
		getRootView()->addChild(view);
	}
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
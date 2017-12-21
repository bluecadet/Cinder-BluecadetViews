#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "bluecadet/core/BaseApp.h"
#include "bluecadet/views/BaseView.h"
#include "bluecadet/views/ImageView.h"
#include "bluecadet/views/TextView.h"
#include "bluecadet/views/TouchView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::core;
using namespace bluecadet::views;
using namespace bluecadet::touch;

class BlendModesSampleApp : public BaseApp {
public:
	static void prepareSettings(ci::app::App::Settings* settings);
	void setup() override;
	void draw() override;
	void addView(std::string title, BaseViewRef view);
	ci::gl::TextureRef loadTextureAsset(std::string relAssetPath);

	ci::Rectf mViewBounds;
	std::set<ci::params::InterfaceGlRef> mParams;
};

void BlendModesSampleApp::prepareSettings(ci::app::App::Settings* settings) {
	SettingsManager::getInstance()->setup(settings, ci::app::getAssetPath("settings.json"), [=](SettingsManager * manager) {
		manager->mFps = 400;
		manager->mConsole = false;
		manager->mVerticalSync = false;
		manager->mMinimizeParams = true;
		manager->mFullscreen = false;
		manager->mDisplaySize = ivec2(1280, 720);
	});
}

void BlendModesSampleApp::setup() {
	BaseApp::setup();

	mViewBounds = ci::Rectf(0, 0, 0, 0);

	// views
	getRootView()->setBackgroundColor(Color::gray(0.5f));

	vec2 defaultSize(256);

	{
		auto view = make_shared<BaseView>();
		view->setBackgroundColor(Color(1.0f, 0, 0));
		view->setSize(defaultSize);
		addView("Basic View", view);
	}

	auto tex = loadTextureAsset("pattern_line_grid_white.png");

	{
		auto view = make_shared<ImageView>();
		view->setTexture(tex);
		addView("White Image ", view);
	}

	{
		auto view = make_shared<ImageView>();
		view->setTexture(loadTextureAsset("pattern_line_grid_black.png"));
		addView("Black Image", view);
	}

	{
		auto view = make_shared<TextView>();
		view->setSize(defaultSize);
		view->setTextColor(Color::white());
		view->setText("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.");
		addView("White Text", view);
	}
}

void BlendModesSampleApp::draw() {
	BaseApp::draw();
}

void BlendModesSampleApp::addView(std::string title, BaseViewRef view) {

	const auto padding = 20.0f;

	// touch container to toggle params
	auto touchContainer = make_shared<TouchView>();
	touchContainer->setSize(view->getSize() + 2.0f * padding);
	view->setPosition((touchContainer->getSize() - view->getSize()) * 0.5f);
	touchContainer->addChild(view);

	// layout
	if (mViewBounds.getSize().x + touchContainer->getWidth() <= getRootView()->getWidth()) {
		if (mViewBounds.getHeight() == 0) {
			touchContainer->setPosition(vec2(0));
		} else {
			touchContainer->setPosition(mViewBounds.getLowerRight() - vec2(0, touchContainer->getHeight()));
		}
	} else {
		touchContainer->setPosition(mViewBounds.getLowerLeft());
		mViewBounds.scale(vec2(0, 1.0f));
	}

	mViewBounds.include(touchContainer->getBounds());

	// params
	auto params = ci::params::InterfaceGl::create(title, ivec2(200, 200));
	
	params->addParam("Blend Mode", {"Inherit", "Alpha", "Premult", "Add", "Multiply"}, [=] (int index) { view->setBlendMode((BaseView::BlendMode)index); }, [=] { return (int)view->getBlendMode(); });
	params->addParam<float>("Alpha", [=](float v) { view->setAlpha(v); }, [=] { return view->getAlphaConst(); }).min(0).max(1.0f).step(1.0f / 255.0f);

	params->setPosition(touchContainer->getPosition().value());
	params->hide();
	touchContainer->getSignalTapped().connect([=](...) {
		if (params->isVisible()) {
			params->hide();
		} else {
			params->show();
		}
	});
	mParams.insert(params);


	getRootView()->addChild(touchContainer);
}

ci::gl::TextureRef BlendModesSampleApp::loadTextureAsset(std::string relAssetPath) {
	auto assetPath = getAssetPath(relAssetPath);
	
	if (assetPath.empty()) {
		return nullptr;
	}

	auto surface = loadImage(assetPath);

	if (!surface) {
		return nullptr;
	}

	gl::Texture::Format format;
	format.enableMipmapping(true);
	format.setMaxMipmapLevel(-1);
	format.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	format.setMagFilter(GL_LINEAR);

	return gl::Texture::create(surface);
}

// Make sure to pass a reference to prepareSettings to configure the app correctly. MSAA and other render options are optional.
CINDER_APP(BlendModesSampleApp, RendererGl(RendererGl::Options().msaa(4)), BlendModesSampleApp::prepareSettings);
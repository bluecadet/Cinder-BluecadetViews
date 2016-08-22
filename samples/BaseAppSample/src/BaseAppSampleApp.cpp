#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseApp.h"
#include "TouchView.h"
#include "ImageView.h"
#include "SettingsManager.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::views;
using namespace bluecadet::utils;
using namespace bluecadet::touch;

class BaseAppSampleApp : public BaseApp {
public:
	static void prepareSettings(ci::app::App::Settings* settings);
	void setup() override;
	void update() override;
};

void BaseAppSampleApp::prepareSettings(ci::app::App::Settings* settings)
{
	BaseApp::prepareSettings(settings);
	settings->setFullScreen(false);
	settings->setWindowSize(1024, 768);
	settings->setBorderless(false);
}

void BaseAppSampleApp::setup()
{
	BaseApp::setup();

	
	
	auto button = TouchViewRef(new TouchView());
	button->setSize(vec2(100.0f, 100.0f));
	button->setBackgroundColor(ColorA(1, 0, 0, 1));
	button->setPosition((vec2(getWindowSize()) - button->getSize()) * 0.5f);
	button->mDidBeginTouch.connect([=](bluecadet::touch::TouchEvent e) { e.target->setAlpha(0.5f); });
	button->mDidEndTouch.connect([=](bluecadet::touch::TouchEvent e) { e.target->setAlpha(1.0f); });

	mRootView->addChild(button);
}

void BaseAppSampleApp::update()
{
	BaseApp::update();
}

CINDER_APP(BaseAppSampleApp, RendererGl, BaseAppSampleApp::prepareSettings);

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "BaseApp.h"
#include "SettingsManager.h"
#include "utils/ViewAnimations.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::views;
using namespace bluecadet::utils;

class ViewAnimationsSampleApp : public BaseApp {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseUp(MouseEvent event) override;
	void update() override;
	void draw() override;
	static void prepareSettings(ci::app::App::Settings* settings);

	BaseViewRef		testView;
};

void ViewAnimationsSampleApp::setup()
{
	BaseApp::setup();
	testView = BaseViewRef(new BaseView());
	testView->setSize(vec2(10, 10));
	testView->setPosition(vec2(5, 5));
	testView->setBackgroundColor(Color(1, 0, 0));
	mRootView->addChild(testView);
}

void ViewAnimationsSampleApp::prepareSettings(ci::app::App::Settings* settings) {
	BaseApp::prepareSettings(settings);
	SettingsManager::getInstance()->mDebugFullscreen = false;
	SettingsManager::getInstance()->mDebugWindowSize = vec2(200, 200);
}

void ViewAnimationsSampleApp::mouseDown( MouseEvent event )
{
	//ViewAnimations::fadeOut(testView, 0.5f);
	testView->getTimeline()->apply(&testView->getAlpha(), 0.0f, 2.0f, EaseInExpo());
}

void ViewAnimationsSampleApp::mouseUp(MouseEvent event)
{
	ViewAnimations::fadeIn(testView, 0.5f);
}

void ViewAnimationsSampleApp::update()
{
	BaseApp::update();
}

void ViewAnimationsSampleApp::draw()
{
	BaseApp::draw();
}

CINDER_APP( ViewAnimationsSampleApp, RendererGl )

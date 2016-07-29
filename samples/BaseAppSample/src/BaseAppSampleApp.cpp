#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseApp.h"
#include "TouchView.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::views;

class BaseAppSampleApp : public BaseApp {
public:
	static void prepareSettings(ci::app::App::Settings* settings);
	void setup() override;
	void update() override;
	void draw() override;

protected:
	TouchViewRef mButton;
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

	mButton = TouchViewRef(new TouchView());
	mButton->setup(vec2(100.0f, 100.0f));
	mButton->setPosition((vec2(getWindowSize()) - mButton->getSize()) * 0.5f);

	mRootView->addChild(mButton);
}

void BaseAppSampleApp::update()
{
	BaseApp::update();
}

void BaseAppSampleApp::draw()
{
	BaseApp::draw();
}

CINDER_APP(BaseAppSampleApp, RendererGl, BaseAppSampleApp::prepareSettings);

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseApp.h"
#include "debug/RectButton.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::views;

class BaseAppSampleApp : public BaseApp {
public:
	void setup() override;
	void update() override;
	void draw() override;

protected:
	debug::RectButtonRef mButton;
};

void BaseAppSampleApp::setup()
{
	BaseApp::setup();

	mButton = bluecadet::views::debug::RectButtonRef(new bluecadet::views::debug::RectButton());
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

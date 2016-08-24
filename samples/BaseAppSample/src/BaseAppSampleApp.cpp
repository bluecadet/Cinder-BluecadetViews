#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseApp.h"
#include "TouchView.h"
#include "ImageView.h"

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
	settings->setWindowSize(1280, 720);
	settings->setBorderless(false);
}

void BaseAppSampleApp::setup()
{

	ScreenLayout::getInstance()->setNumRows(3);
	ScreenLayout::getInstance()->setNumColumns(4);

	BaseApp::setup();
	BaseApp::addTouchSimulatorParams();

	getRootView()->setBackgroundColor(Color::gray(0.5f));
	getRootView()->setSize(ScreenLayout::getInstance()->getAppSize());

	auto addButton = [=](vec2 pos, vec2 size, ColorA color) {
		auto button = TouchViewRef(new TouchView());
		button->setSize(size);
		button->setBackgroundColor(color);
		button->setPosition(pos);
		button->mDidTap.connect([=](bluecadet::touch::TouchEvent e) { CI_LOG_I("Button tapped"); });
		getRootView()->addChild(button);
	};

	const vec2 buttonSize = getRootView()->getSize() * 0.5f;
	addButton(getRootView()->getSize() * 0.5f + (vec2(-buttonSize.x, -buttonSize.y) - buttonSize) * 0.5f, buttonSize, ColorA(1, 0, 0, 1));
	addButton(getRootView()->getSize() * 0.5f + (vec2(buttonSize.x, -buttonSize.y) - buttonSize) * 0.5f, buttonSize, ColorA(1, 1, 0, 1));
	addButton(getRootView()->getSize() * 0.5f + (vec2(buttonSize.x, buttonSize.y) - buttonSize) * 0.5f, buttonSize, ColorA(0, 0, 1, 1));
	addButton(getRootView()->getSize() * 0.5f + (vec2(-buttonSize.x, buttonSize.y) - buttonSize) * 0.5f, buttonSize, ColorA(0, 1, 0, 1));
}

void BaseAppSampleApp::update()
{
	BaseApp::update();
}

CINDER_APP(BaseAppSampleApp, RendererGl, BaseAppSampleApp::prepareSettings);

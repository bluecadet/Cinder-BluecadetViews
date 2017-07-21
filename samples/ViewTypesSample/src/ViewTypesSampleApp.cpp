#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#include "bluecadet/core/BaseApp.h"
#include "bluecadet/views/BaseView.h"
#include "bluecadet/views/EllipseView.h"
#include "bluecadet/views/FboView.h"
#include "bluecadet/views/LineView.h"
#include "bluecadet/views/TouchView.h"
#include "bluecadet/views/TextView.h"


using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::core;
using namespace bluecadet::views;
using namespace bluecadet::touch;

class ViewTypesSampleApp : public BaseApp {
public:
	static void prepareSettings(ci::app::App::Settings* settings);
	
	void setup() override;
	void update() override;
	void draw() override;
	void keyDown(ci::app::KeyEvent event) override;
	void addViewSample(BaseViewRef view, std::string label);
};

void ViewTypesSampleApp::prepareSettings(ci::app::App::Settings* settings) {
	SettingsManager::getInstance()->setup(settings, "", [](SettingsManager * manager) {
		manager->mFullscreen = false;
		manager->mWindowSize = ivec2(1280, 720);
		manager->mConsoleWindowEnabled = false;
		manager->mDrawMinimap = true;
		manager->mDrawStats = true;
		manager->mDrawTouches = true;
		manager->mMinimizeParams = true;
	});
}

void ViewTypesSampleApp::setup() {
	BaseApp::setup();

	//==================================================
	// Most basic view
	// 

	auto view = BaseViewRef(new BaseView());
	view->setSize(vec2(100, 100));
	view->setBackgroundColor(ColorA(1.0f, 0.5f, 0.5f, 0.75f));
	addViewSample(view, "BaseView");


	//==================================================
	// EllipseView with variably smooth edges
	// 

	auto ellipseView = EllipseViewRef(new EllipseView());
	ellipseView->setSize(vec2(150, 100)); // if width/height are equal you can also use setRadius()
	ellipseView->setPosition(ellipseView->getSize() * 0.5f); // ellipse is drawn around 0,0; so offset by 50% width/height
	ellipseView->setBackgroundColor(Color(0, 1.0f, 0.5f));
	ellipseView->setSmoothness(1.0f); // the default is 1
	addViewSample(ellipseView, "EllipseView");

	// test smoothness update
	getSignalUpdate().connect([=] { ellipseView->setSmoothness(50.0f * getMousePos().x / (float)getWindowWidth()); });


	//==================================================
	// LineView
	// 

	auto lineView = LineViewRef(new LineView());
	lineView->setEndPoint(vec2(100, 100));
	lineView->setLineColor(ColorA(1.0f, 0.0f, 1.0f, 1.0f));
	lineView->setLineWidth(2.0f);
	addViewSample(lineView, "LineView");


	//==================================================
	// TouchViews with various hit areas
	// 

	auto touchView = TouchViewRef(new TouchView());
	touchView->setDebugDrawTouchPath(true);
	touchView->setSize(vec2(200, 100));
	touchView->setTransformOrigin(0.5f * touchView->getSize());
	touchView->setBackgroundColor(ColorA(0, 1.0f, 0, 1.0f));
	touchView->getSignalTouchBegan().connect([=](const bluecadet::touch::TouchEvent& e) { touchView->resetAnimations(); touchView->setScale(1.5f); });
	touchView->getSignalTouchEnded().connect([=](const bluecadet::touch::TouchEvent& e) { touchView->getTimeline()->apply(&touchView->getScale(), vec2(1.0f), 0.3f); });
	addViewSample(touchView, "TouchView with TouchBegan/Ended");

	auto diamondTouchView = TouchViewRef(new TouchView());
	diamondTouchView->setDebugDrawTouchPath(true);
	diamondTouchView->setSize(vec2(200, 100));
	diamondTouchView->setTransformOrigin(0.5f * diamondTouchView->getSize());
	diamondTouchView->setup([] {
		ci::Path2d p;
		p.moveTo(50, 0);
		p.lineTo(100, 50);
		p.lineTo(50, 100);
		p.lineTo(0, 50);
		p.close();
		return p;
	}());
	diamondTouchView->setBackgroundColor(ColorA(0, 0, 1.0f, 1.0f));
	diamondTouchView->getSignalTouchBegan().connect([=](const bluecadet::touch::TouchEvent& e) { diamondTouchView->resetAnimations(); diamondTouchView->setScale(1.5f); });
	diamondTouchView->getSignalTouchEnded().connect([=](const bluecadet::touch::TouchEvent& e) { diamondTouchView->getTimeline()->apply(&diamondTouchView->getScale(), vec2(1.0f), 0.3f); });
	addViewSample(diamondTouchView, "TouchView with diamond Touch Path");

	const float circleTouchRadius = 50.0f;
	auto circleTouchView = TouchViewRef(new TouchView());
	circleTouchView->setDebugDrawTouchPath(true);
	circleTouchView->setSize(vec2(200, 100));
	circleTouchView->setTransformOrigin(0.5f * circleTouchView->getSize());
	circleTouchView->setup(circleTouchRadius, vec2(circleTouchRadius));
	circleTouchView->setBackgroundColor(ColorA(0, 1.0f, 1.0f, 1.0f));
	circleTouchView->getSignalTouchBegan().connect([=](const bluecadet::touch::TouchEvent& e) { circleTouchView->resetAnimations(); circleTouchView->setScale(1.5f); });
	circleTouchView->getSignalTouchEnded().connect([=](const bluecadet::touch::TouchEvent& e) { circleTouchView->getTimeline()->apply(&circleTouchView->getScale(), vec2(1.0f), 0.3f); });
	addViewSample(circleTouchView, "TouchView with circle Touch Path");

	//==================================================
	// FBO
	// 

	auto fboView = FboViewRef(new FboView());
	fboView->setup(vec2(150));
	fboView->setBackgroundColor(ColorA(0.0f, 0.0f, 1.0f, 0.5f));

	auto circleInsideFbo = EllipseViewRef(new EllipseView());
	circleInsideFbo->setup(100.0f, ColorA(0.8f, 0.2f, 0.2f, 1.0f));
	circleInsideFbo->getTimeline()->apply(&circleInsideFbo->getScale(), vec2(3.0f), 2.0f).loop(true);
	fboView->addChild(circleInsideFbo);

	addViewSample(fboView, "FBOView with circle inside");


	//==================================================
	// Drag Views
	// 
	{
		// x and y
		auto dragView = TouchViewRef(new TouchView());
		dragView->setSize(vec2(100, 100));
		dragView->setBackgroundColor(ColorA(1.0f, 0.5f, 0.5f, 0.75f));
		dragView->setDragEnabled(true);
		addViewSample(dragView, "TouchView with x/y drag");
	}


	{
		// x only
		auto dragView = TouchViewRef(new TouchView());
		dragView->setSize(vec2(100, 100));
		dragView->setBackgroundColor(ColorA(0.5f, 1.0f, 0.5f, 0.75f));
		dragView->setDragEnabledX(true);
		addViewSample(dragView, "TouchView with x drag");
	}

	{
		// y only
		auto dragView = TouchViewRef(new TouchView());
		dragView->setSize(vec2(100, 100));
		dragView->setBackgroundColor(ColorA(0.5f, 0.5f, 1.0f, 0.75f));
		dragView->setDragEnabledY(true);
		addViewSample(dragView, "TouchView with y drag");
	}

	getRootView()->setBackgroundColor(Color::gray(0.5f));
}

void ViewTypesSampleApp::keyDown(ci::app::KeyEvent event) {
	switch (event.getCode()) {
	case KeyEvent::KEY_ESCAPE:
		for (auto child : getRootView()->getChildren()) {
			if (auto touchView = dynamic_pointer_cast<TouchView>(child)) {
				touchView->cancelTouches();
			}
		}
		break;
	default:
		break;
	}
}

void ViewTypesSampleApp::addViewSample(BaseViewRef view, std::string label) {

	// hacky layout
	static const int numCols = 4;
	static const int numRows = 3;
	static const vec2 cellPadding = vec2(10);
	static const vec2 cellSize = (vec2(getWindowSize()) - vec2(numCols + 1, numRows + 1) * cellPadding) / vec2(numCols, numRows);
	static vec2 cellPos = cellPadding;

	auto container = make_shared<BaseView>();
	container->setSize(cellSize);
	container->setPosition(cellPos);
	container->setBackgroundColor(Color::black());
	getRootView()->addChild(container);

	vec2 viewOffset = view->getPosition();
	view->setPosition((container->getSize() - view->getSize()) * 0.5f + viewOffset);
	container->addChild(view);

	auto labelView = make_shared<TextView>();
	labelView->setWidth(container->getWidth());
	labelView->setFontSize(20.0f);
	labelView->setTextColor(Color::white());
	labelView->setTextAlign(bluecadet::text::TextAlign::Center);
	labelView->setText(label);
	container->addChild(labelView);

	cellPos.x += cellSize.x + cellPadding.x;

	if (cellPos.x >= getWindowWidth()) {
		cellPos.x = cellPadding.x;
		cellPos.y += cellSize.y + cellPadding.y;
	}
}

void ViewTypesSampleApp::update() {
	BaseApp::update();
}

void ViewTypesSampleApp::draw() {
	BaseApp::draw();
}

CINDER_APP(ViewTypesSampleApp, RendererGl(RendererGl::Options().msaa(4)), ViewTypesSampleApp::prepareSettings)
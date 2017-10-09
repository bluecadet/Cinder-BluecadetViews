#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#include "bluecadet/core/BaseApp.h"

#include "bluecadet/views/ArcView.h"
#include "bluecadet/views/BaseView.h"
#include "bluecadet/views/EllipseView.h"
#include "bluecadet/views/FboView.h"
#include "bluecadet/views/MaskView.h"
#include "bluecadet/views/LineView.h"
#include "bluecadet/views/StrokedCircleView.h"
#include "bluecadet/views/StrokedRectView.h"
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
	void keyDown(ci::app::KeyEvent event) override;
	void addViewSample(BaseViewRef view, std::string label);
	ColorA getNextColor(float alpha = 1.0f);
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

	auto view = make_shared<BaseView>();
	view->setSize(vec2(100, 100));
	view->setBackgroundColor(getNextColor());
	addViewSample(view, "BaseView");

	//==================================================
	// Stroked rect view
	// 

	auto strokedRectView = make_shared<StrokedRectView>();
	strokedRectView->setSize(vec2(100, 100));
	strokedRectView->setBackgroundColor(getNextColor());
	strokedRectView->setStrokeColor(ColorA(getNextColor(), 0.75f));
	strokedRectView->setStrokeWidth(5.0f);
	strokedRectView->getTimeline()->apply(&strokedRectView->getStrokeWidth(), 20.0f, 1.5f, easeInOutQuad).loop(true).pingPong(true);
	addViewSample(strokedRectView, "StrokedRectView");

	//==================================================
	// EllipseView with variably smooth edges
	// 

	auto ellipseView = make_shared<EllipseView>();
	ellipseView->setSize(vec2(150, 100)); // if width/height are equal you can also use setRadius()
	ellipseView->setPosition(ellipseView->getSize() * 0.5f); // ellipse is drawn around 0,0; so offset by 50% width/height
	ellipseView->setBackgroundColor(getNextColor());
	ellipseView->setSmoothness(1.0f); // the default is 1
	ellipseView->getTimeline()->apply(&ellipseView->getSmoothness(), 50.0f, 3.0f, easeInOutQuad).loop(true).pingPong(true);
	addViewSample(ellipseView, "EllipseView");

	//==================================================
	// Stroked Circle
	// 

	auto strokedCircle = make_shared<StrokedCircleView>();
	strokedCircle->setBackgroundColor(getNextColor());
	strokedCircle->setStrokeColor(getNextColor());
	strokedCircle->setStrokeWidth(5.0f);
	strokedCircle->setSmoothness(1.0f);
	strokedCircle->setRadius(75.0f);
	strokedCircle->setPosition(strokedCircle->getSize() * 0.5f);
	strokedCircle->getTimeline()->apply(&strokedCircle->getSmoothness(), 10.0f, 2.0f, easeInOutQuad).loop(true).pingPong(true);
	addViewSample(strokedCircle, "StrokedCircleView");


	//==================================================
	// TouchViews with various hit areas
	// 

	auto tapView = make_shared<TouchView>();
	tapView->setSize(vec2(200, 100));
	tapView->setTransformOrigin(0.5f * tapView->getSize());
	tapView->setBackgroundColor(getNextColor());
	tapView->getSignalTouchBegan().connect([=](...) { tapView->setTint(Color::gray(0.75f)); });
	tapView->getSignalTouchEnded().connect([=](...) { tapView->setTint(Color::white()); });
	tapView->getSignalTapped().connect([=](...) {
		static float rotation = 0;
		rotation += (float)M_PI * 0.33f;
		tapView->getTimeline()->apply(&tapView->getRotation(), (glm::angleAxis(rotation, vec3(0, 0, 1))), 0.33f, easeOutQuad);
	});
	addViewSample(tapView, "TouchView with tap<br>(long press and drag-and-release don't count as tap)");

	auto diamondTouchView = make_shared<TouchView>();
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
	diamondTouchView->setBackgroundColor(getNextColor());
	diamondTouchView->getSignalTouchBegan().connect([=](const bluecadet::touch::TouchEvent& e) { diamondTouchView->cancelAnimations(); diamondTouchView->setScale(1.5f); });
	diamondTouchView->getSignalTouchEnded().connect([=](const bluecadet::touch::TouchEvent& e) { diamondTouchView->getTimeline()->apply(&diamondTouchView->getScale(), vec2(1.0f), 0.3f); });
	addViewSample(diamondTouchView, "TouchView with diamond touch path");

	const float circleTouchRadius = 50.0f;
	auto circleTouchView = make_shared<TouchView>();
	circleTouchView->setDebugDrawTouchPath(true);
	circleTouchView->setSize(vec2(200, 100));
	circleTouchView->setTransformOrigin(0.5f * circleTouchView->getSize());
	circleTouchView->setup(circleTouchRadius, vec2(circleTouchRadius));
	circleTouchView->setBackgroundColor(getNextColor());
	circleTouchView->getSignalTouchBegan().connect([=](const bluecadet::touch::TouchEvent& e) { circleTouchView->cancelAnimations(); circleTouchView->setScale(1.5f); });
	circleTouchView->getSignalTouchEnded().connect([=](const bluecadet::touch::TouchEvent& e) { circleTouchView->getTimeline()->apply(&circleTouchView->getScale(), vec2(1.0f), 0.3f); });
	addViewSample(circleTouchView, "TouchView with circle touch path");

	//==================================================
	// Drag Views
	// 
	{
		// x and y
		auto dragView = make_shared<TouchView>();
		dragView->setSize(vec2(100));
		dragView->setBackgroundColor(getNextColor());
		dragView->setDragEnabled(true);
		addViewSample(dragView, "TouchView with x/y drag");
	}


	{
		// x only
		auto dragView = make_shared<TouchView>();
		dragView->setSize(vec2(30, 60));
		dragView->setBackgroundColor(getNextColor());
		dragView->setDragEnabledX(true);
		addViewSample(dragView, "TouchView with x drag");
	}

	{
		// y only
		auto dragView = make_shared<TouchView>();
		dragView->setSize(vec2(60, 30));
		dragView->setBackgroundColor(Color(0.25f, 0, 1.0f));
		dragView->setDragEnabledY(true);
		addViewSample(dragView, "TouchView with y drag");
	}

	getRootView()->setBackgroundColor(Color::gray(0.5f));

	//==================================================
	// FBO
	// 

	auto fboView = make_shared<FboView>();
	fboView->setup(vec2(150));
	fboView->setBackgroundColor(getNextColor());

	auto circleInsideFbo = make_shared<EllipseView>();
	circleInsideFbo->setup(length(fboView->getSize()), getNextColor());
	circleInsideFbo->getTimeline()->apply(&circleInsideFbo->getScale(), vec2(0), 3.0f, easeInOutQuad).pingPong(true).loop(true);
	fboView->addChild(circleInsideFbo);

	addViewSample(fboView, "FBOView with circle inside");

	//==================================================
	// Masks
	// 

	{
		// Reveal
		auto maskView = make_shared<MaskView>();
		maskView->setSize(vec2(150));
		maskView->setMaskType(MaskView::MaskType::REVEAL); // this is the default
		maskView->setBackgroundColor(getNextColor());
		addViewSample(maskView, "MaskView that reveals masked content");

		auto mask = make_shared<EllipseView>();
		mask->setup(length(fboView->getSize()), getNextColor());
		mask->getTimeline()->apply(&mask->getScale(), vec2(0), 3.0f, easeInOutQuad).pingPong(true).loop(true);
		maskView->setMask(mask);
	}

	{
		// Hide
		auto maskView = make_shared<MaskView>();
		maskView->setSize(vec2(150));
		maskView->setMaskType(MaskView::MaskType::HIDE);
		maskView->setBackgroundColor(getNextColor());
		addViewSample(maskView, "MaskView that hides masked content");

		auto mask = make_shared<EllipseView>();
		mask->setup(length(fboView->getSize()), getNextColor());
		mask->getTimeline()->apply(&mask->getScale(), vec2(0), 3.0f, easeInOutQuad).pingPong(true).loop(true);
		maskView->setMask(mask);
	}

	//==================================================
	// LineView
	// 

	auto lineView = make_shared<LineView>();
	lineView->setEndPoint(vec2(100, 100));
	lineView->setLineColor(getNextColor());
	lineView->setLineWidth(2.0f);
	addViewSample(lineView, "LineView");

	//==================================================
	// Arc
	// 

	auto arcView = make_shared<ArcView>();
	arcView->setup(5, 50, 0, glm::two_pi<float>(), getNextColor());
	arcView->setPosition(arcView->getSize() * 0.5f); // arc is drawn around 0,0; so offset by 50% width/height
	arcView->getTimeline()->apply(&arcView->getSmoothness(), 10.0f, 1.5f, easeInOutQuad).loop(true).pingPong(true);
	arcView->getTimeline()->apply(&arcView->getInnerRadius(), 45.0f, 2.4f, easeInOutQuad).loop(true).pingPong(true);
	arcView->getTimeline()->apply(&arcView->getStartAngle(), glm::pi<float>(), 2.0f, easeInOutQuad).loop(true).pingPong(true);
	arcView->getTimeline()->apply(&arcView->getEndAngle(), glm::pi<float>(), 2.8f, easeInOutQuad).loop(true).pingPong(true);
	addViewSample(arcView, "ArcView");
}

void ViewTypesSampleApp::keyDown(ci::app::KeyEvent event) {
	BaseApp::keyDown(event);

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
	static const int numCols = 5;
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
	labelView->setBackgroundColor(ColorA(0, 0, 0, 0.25f));
	labelView->setPadding(cellPadding.x, cellPadding.y);
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

ColorA ViewTypesSampleApp::getNextColor(float alpha) {
	static float hue = 0;
	static float numHues = 16;
	ColorA color = ColorA(hsvToRgb(vec3(hue, 0.75f, 1.0f)), alpha);
	hue = glm::mod(hue + 0.25f + 1.0f / numHues, 1.0f);
	return color;
}

CINDER_APP(ViewTypesSampleApp, RendererGl(RendererGl::Options().msaa(4).stencil(true)), ViewTypesSampleApp::prepareSettings)
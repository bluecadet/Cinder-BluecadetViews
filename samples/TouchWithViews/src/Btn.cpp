#include "Btn.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::views;

//==================================================
// Class Lifecycle
// 

Btn::Btn() : TouchView() {
}

Btn::~Btn(){
}

void Btn::setup() {
	float radius = 40.0f;

	// Setup touchable area, circle btn
	TouchView::setup(radius);
	
	// Create bg
	mBg = EllipseViewRef(new EllipseView());
	mBg->setBackgroundColor(Color::white());
	mBg->setRadius(radius);
	mBg->setAlpha(0.5f);

	// Add views
	addChild(mBg);
}

void Btn::update(const double deltaTime) {
	// Drag this view to match the location of the current touch if it has moving touches
	// *Another way to do this could be to make a handleMovedTouch() signal callback instead of having this check in the update
	if (hasMovingTouches()) {
		setPosition(getCurTouchPosition());
	}
}

//==================================================
// Callbacks
// 

void Btn::handleTouchBegan(const bluecadet::touch::TouchEvent& touchEvent) {
	// Set hit state immediately on touch down
	getTimeline()->removeTarget(&mBg->getAlpha());
	mBg->setAlpha(1.0f);
}

void Btn::handleTouchEnded(const bluecadet::touch::TouchEvent& touchEvent) {
	// Remove hit state gradually on touch up
	getTimeline()->apply(&mBg->getAlpha(), 0.5f, 0.25f, easeOutQuad);
}

void Btn::handleTouchCanceled(const bluecadet::touch::TouchEvent& touchEvent) {
	// Remove hit state immediately on cancel
	getTimeline()->removeTarget(&mBg->getAlpha());
	mBg->setAlpha(0.5f);
}
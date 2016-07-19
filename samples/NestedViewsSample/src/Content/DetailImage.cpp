#include "DetailImage.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::views;

//==================================================
// Class Lifecycle
// 

DetailImage::DetailImage() : TouchView(true) {
}

DetailImage::~DetailImage(){
}

void DetailImage::setup(int id) {
	
	mBgImageContainer = BaseViewRef( new BaseView() );

	// Create bg
	mBgImage = ImageViewRef(new ImageView());
	string iconFileName = "bluecadetIcon.png";
	mBgImage->setup(iconFileName, vec2(0));
	// Center the bgImage within the container
	mBgImage->setPosition(vec2(-mBgImage->getWidth()*0.5f, -mBgImage->getHeight()*0.5f));
	// Move the container back
	mBgImageContainer->setPosition(vec2(mBgImage->getWidth()*0.5f, mBgImage->getHeight()*0.5f));

	// Add views
	mBgImageContainer->addChild(mBgImage);
	addChild(mBgImageContainer);

	// Setup touchable area, rectangle based on icon size
	TouchView::setup(vec2(mBgImage->getWidth(), mBgImage->getHeight()));
	setTouchEnabled(true);

	// Set the position of the entire detailImage (touchable area + bgImageContainer)
	float xPos = mBgImage->getWidth()*(float)id;
	float yPos = 0.0f;
	int maxColumns = 5;
	if (id > maxColumns){
		xPos -= maxColumns*mBgImage->getWidth();
		yPos += mBgImage->getHeight();
	}

	// Set the alpha of every other image
	if (id % 2 == 0)
		setAlpha(0.5f);

	setPosition(vec2(xPos, yPos));
}

void DetailImage::update() {
	// Drag this view to match the location of the current touch if it has moving touches
	// *Another way to do this could be to make a handleMovedTouch() signal callback instead of having this check in the update
	if (hasMovingTouches()) {
		setPosition(getCurTouchPosition());
	}
}

void DetailImage::draw() {
	// Debugging use only: Draw the shape of the touchable area of this button to compare against the visible shapes you draw within
	drawDebugShape();
}

//==================================================
// Callbacks
// 

void DetailImage::handleTouchBegan(const bluecadet::touch::TouchEvent& touchEvent) {
	// Set hit state immediately on touch down
	mBgImageContainer->setScale(vec2(1.25f));
}

void DetailImage::handleTouchEnded(const bluecadet::touch::TouchEvent& touchEvent) {
	// Remove hit state gradually on touch up
	getTimeline()->apply(&mBgImageContainer->getScale(), vec2(1), 0.25f, easeOutQuad);
}

void DetailImage::handleTouchCanceled(const bluecadet::touch::TouchEvent& touchEvent) {
	// Remove hit state immediately on cancel
	getTimeline()->removeTarget(&mBgImageContainer->getScale());
	mBgImage->setScale(vec2(1.0f));
}
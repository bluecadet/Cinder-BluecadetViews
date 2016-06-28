#include "ImageView.h"
#include "ImageManager.h"
#include "cinder/Timeline.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

//==================================================
// Class Lifecycle
// 
ImageView::ImageView() : BaseView(),
mTexture(nullptr),
mSize(0),
mDrawingArea()
{
}

ImageView::~ImageView() {
}

void ImageView::setup(const std::string &fileName, const ci::vec2 &size) {
	setup(ImageManager::getInstance()->getTexture(fileName), size);
}

void ImageView::reset() {
	mTexture = nullptr;
	mSize = vec2();
	mDrawingDestRect = Rectf();
	mDrawingArea = Area();
}

void ImageView::stopAnimation() {
	resetAnimations();
}

void ImageView::setup(const gl::TextureRef texture, const ci::vec2 &size) {
	mTexture = texture;

	if (size == vec2(0) && mTexture) {
		mSize = vec2(mTexture->getWidth(), mTexture->getHeight());
	}
	else mSize = size;

	mDrawingDestRect = Rectf(0.0f, 0.0f, mSize.x, mSize.y);

	if (mTexture) {
		// aspect fill drawing area
		mDrawingArea = Area(mDrawingDestRect.getCenteredFit(mTexture->getBounds(), true));
	}
}

void ImageView::animateOn(float alpha, float aniDur, float aniDelay) {
	getTimeline()->apply(&getAlpha(), alpha, aniDur, easeOutQuad)
		.delay(aniDelay);
}

void ImageView::animateOff(float alpha, float aniDur, float aniDelay) {
	getTimeline()->apply(&getAlpha(), alpha, aniDur, easeOutQuad)
		.delay(aniDelay);
}

void ImageView::drawContent() {

	if (!mTexture) {
		return;
	}

	gl::draw(mTexture, mDrawingArea, mDrawingDestRect);
}

} 
} 
#include "ImageView.h"
#include "ImageManager.h"
#include "cinder/Timeline.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::utils;

namespace bluecadet {
namespace views {

ImageView::ImageView() : BaseView(),
mTexture(nullptr),
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
	mDrawingDestRect = Rectf();
	mDrawingArea = Area();
}

void ImageView::stopAnimation() {
	resetAnimations();
}

void ImageView::setup(const gl::TextureRef texture, const ci::vec2 &size) {
	mTexture = texture;

	if ((size.x == 0 || size.y == 0) && mTexture) {
		setSize(vec2(mTexture->getWidth(), mTexture->getHeight()));
	} else {
		setSize(size);
	}

	mDrawingDestRect = Rectf(0.0f, 0.0f, getWidth(), getHeight());

	if (mTexture) {
		// Aspect fill drawing area
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

void ImageView::draw() {
	if (!mTexture) return;
	gl::draw(mTexture, mDrawingArea, mDrawingDestRect);
}

}
}
#include "ImageView.h"
#include "cinder/Timeline.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

ImageView::ImageView() : BaseView(),
mTexture(nullptr),
mScaleMode(ScaleMode::NONE)
{
}

ImageView::~ImageView() {
}

void ImageView::reset() {
    BaseView::reset();
    clearTexture();
}

void ImageView::clearTexture() {
    mTexture = nullptr;
    mDrawingDestRect = Rectf();
    mDrawingArea = Area();
	mScaleMode = ScaleMode::NONE;
}

void ImageView::setup(const gl::TextureRef texture, const ci::vec2 &size, const ScaleMode scaleMode) {
	const bool resizeToTexture = size.x == 0 && size.y == 0;

	setScaleMode(scaleMode);
	
	setTexture(texture, resizeToTexture);

	if (!resizeToTexture) {
		setSize(size);
	}
}

void ImageView::setTexture(ci::gl::TextureRef texture, const bool resizeToTexture) {
	mTexture = texture;

	if (resizeToTexture) {
		if (mTexture) {
			// apply texture size
			setSize(vec2(mTexture->getSize()));
		} else {
			// reset to 0
			setSize(vec2());
		}
	}

	invalidate(false, true);
}

void ImageView::setSize(const ci::vec2& size) {
	BaseView::setSize(size);

	mDrawingDestRect = Rectf(vec2(0), size);

	if (mTexture) {
		// Aspect fill drawing area
		mDrawingArea = Area(mDrawingDestRect.getCenteredFit(mTexture->getBounds(), true));
	} else {
		mDrawingArea = Area();
	}
}

void ImageView::draw() {
	if (!mTexture) return;
	
	switch (mScaleMode) {
		case ScaleMode::NONE:
			gl::draw(mTexture);
			break;
		case ScaleMode::STRETCH:
			gl::draw(mTexture, mDrawingDestRect);
			break;
		case ScaleMode::FIT:
			gl::draw(mTexture, Rectf(mTexture->getBounds()).getCenteredFit(mDrawingDestRect, true));
			break;
		case ScaleMode::COVER:
			gl::draw(mTexture, mDrawingArea, mDrawingDestRect);
			break;
	}
}

}
}

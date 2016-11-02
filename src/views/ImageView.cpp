#include "ImageView.h"
#include "cinder/Timeline.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

ImageView::ImageView() : BaseView(),
mTexture(nullptr),
mAutoSizeToTexture(true)
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
}

void ImageView::setup(const gl::TextureRef texture, const ci::vec2 &size) {
	setTexture(texture);

	if (size.x != 0 && size.y != 0) {
		setSize(size);
	}
}

void ImageView::setTexture(ci::gl::TextureRef texture) {
	mTexture = texture;

	if (mAutoSizeToTexture) {
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
	gl::draw(mTexture, mDrawingArea, mDrawingDestRect);
}

}
}

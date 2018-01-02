#include "ImageView.h"
#include "cinder/Timeline.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

ImageView::ScaleMode ImageView::sDefaultScaleMode = ImageView::ScaleMode::STRETCH;

ImageView::ImageView() : BaseView(),
mTexture(nullptr),
mScaleMode(sDefaultScaleMode),
mTextureSourceArea(0, 0, 0, 0),
mTextureDestRect(0, 0, 0, 0)
{
}

ImageView::~ImageView() {
}

void ImageView::reset() {
    BaseView::reset();
    setTexture(nullptr);
	setScaleMode(sDefaultScaleMode);
	mTextureSourceArea = Area();
	mTextureDestRect = Rectf();
}

inline void ImageView::setTexture(ci::gl::TextureRef texture, const bool resizeToTexture) {
	mTexture = texture;

	if (resizeToTexture) {
		if (mTexture) {
			setSize(vec2(mTexture->getSize()));
		} else {
			setSize(vec2());
		}
	}

	invalidate(false, true);
}

void ImageView::validateContent() {
	BaseView::validateContent();

	if (!mTexture) {
		mTextureDestRect = Rectf();
		mTextureSourceArea = Area();
		return;
	}

	switch (mScaleMode) {
		case ScaleMode::NONE:
			mTextureDestRect = Rectf(vec2(), getSize());
			mTextureSourceArea = Area(mTextureDestRect);
			break;
		case ScaleMode::STRETCH:
			mTextureDestRect = Rectf(vec2(), getSize());
			mTextureSourceArea = Area(mTexture->getBounds());
			break;
		case ScaleMode::FIT:
			mTextureDestRect = Rectf(mTexture->getBounds()).getCenteredFit(Rectf(vec2(), getSize()), true);
			mTextureSourceArea = Area(mTexture->getBounds());
			break;
		case ScaleMode::COVER:
			mTextureDestRect = Rectf(vec2(), getSize());
			mTextureSourceArea = Area(mTextureDestRect.getCenteredFit(mTexture->getBounds(), true));
			break;
		case ScaleMode::CUSTOM:
			// do nothing
			break;
	}
}

void ImageView::draw() {
	BaseView::draw();

	if (mTexture) {
		gl::draw(mTexture, mTextureSourceArea, mTextureDestRect);
	}
}

}
}

#pragma once

//+---------------------------------------------------------------------------
//  Bluecadet Interactive 2016
//  Contents:
//  Comments:
//----------------------------------------------------------------------------

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseView.h"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class ImageView> ImageViewRef;

class ImageView : public BaseView {

public:

	enum class ScaleMode {
		NONE,
		STRETCH,
		FIT,
		COVER
	};

	ImageView();
	virtual ~ImageView();

	void reset() override;
    virtual void clearTexture();

	virtual void setup(const ci::gl::TextureRef texture, const ci::vec2& size = ci::vec2(0), const ScaleMode scaleMode = ScaleMode::COVER);

	void setSize(const ci::vec2& size) override;

	inline ci::gl::TextureRef getTexture() const { return mTexture; }
	inline void			setTexture(const ci::gl::TextureRef value, const bool resizeToTexture = true);

	inline ScaleMode	getScaleMode() const { return mScaleMode; }
	inline void			setScaleMode(const ScaleMode scaleMode) { mScaleMode = scaleMode; }

private:

	void draw() override;

	ci::gl::TextureRef	mTexture;
	ci::Rectf			mDrawingDestRect;
	ci::Area			mDrawingArea;
	ScaleMode			mScaleMode;
};

}
}

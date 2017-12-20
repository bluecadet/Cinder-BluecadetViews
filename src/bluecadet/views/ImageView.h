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

	enum DrawOptions {
		NONE = 0,
		PREMULTIPLY = 1 << 1,
		DEMULTIPLY = 1 << 2
	};

	ImageView();
	virtual ~ImageView();

	void reset() override;

	inline ci::gl::TextureRef getTexture() const { return mTexture; }
	inline void			setTexture(const ci::gl::TextureRef value, const bool resizeToTexture = true);

	//! Defaults to getDefaultScaleMode()
	inline ScaleMode	getScaleMode() const { return mScaleMode; }
	inline void			setScaleMode(const ScaleMode scaleMode) { mScaleMode = scaleMode; invalidate(false, true); }

	//! Defaults to STRETCH
	static ScaleMode	getDefaultScaleMode() { return sDefaultScaleMode; }
	static void			setDefaultScaleMode(const ScaleMode scaleMode) { sDefaultScaleMode = scaleMode; }

	static void drawTexture(ci::gl::TextureRef texture, BlendMode blendMode, ci::vec2 scale = ci::vec2(1.0f), ci::vec2 size = ci::vec2(-1.0f));
	static void drawTexture(ci::gl::TextureRef texture, DrawOptions options = DrawOptions::NONE, ci::vec2 scale = ci::vec2(1.0f), ci::vec2 size = ci::vec2(-1.0f));

private:

	void draw() override;
	void validateContent() override;
	
	static ScaleMode	sDefaultScaleMode;

	ci::gl::TextureRef	mTexture;
	ci::vec2			mTextureScale;
	ci::vec2			mTextureSize;
	ScaleMode			mScaleMode;
};

}
}

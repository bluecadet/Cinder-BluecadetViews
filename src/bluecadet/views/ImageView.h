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
		NONE,		//! Unscaled within bounds
		STRETCH,	//! Stretched within bounds (default)
		FIT,		//! Scaled to fit within bounds and preserve aspect ratio. Will not crop content.
		COVER,		//! Scaled to fill bounds and preserve aspect ratio. Might crop content.
		CUSTOM		//! Uses the current source area and dest rect.
	};

	ImageView();
	virtual ~ImageView();

	void reset() override;

	inline void			setTexture(const ci::gl::TextureRef value, const bool resizeToTexture = true);
	inline const ci::gl::TextureRef getTexture() const { return mTexture; }

	static void			setDefaultScaleMode(const ScaleMode scaleMode) { sDefaultScaleMode = scaleMode; }
	static ScaleMode	getDefaultScaleMode() { return sDefaultScaleMode; }	//! Defaults to STRETCH
	
	inline void			setScaleMode(const ScaleMode scaleMode) { mScaleMode = scaleMode; invalidate(false, true); }
	inline ScaleMode	getScaleMode() const { return mScaleMode; }	//! Defaults to getDefaultScaleMode()

	//! Configure a custom source area for the texture in texture pixel coordinate space. Will set the scale mode to CUSTOM.
	inline void					setTextureSourceArea(const ci::Area & area) { mTextureSourceArea = area; mScaleMode = ScaleMode::CUSTOM; invalidate(false, true); };
	inline const ci::Area &		getTextureSourceArea() const { return mTextureSourceArea; };

	//! Configure a custom destination rect for the texture in local coordinate space. Will set the scale mode to CUSTOM.
	inline void					setTextureDestRect(const ci::Rectf & rect) { mTextureDestRect = rect; mScaleMode = ScaleMode::CUSTOM; invalidate(false, true); };
	inline const ci::Rectf &	getTextureDestRect() const { return mTextureDestRect; };

private:

	void draw() override;
	void validateContent() override;
	
	static ScaleMode	sDefaultScaleMode;

	ci::gl::TextureRef	mTexture;

	ScaleMode			mScaleMode;
	ci::Area			mTextureSourceArea;
	ci::Rectf			mTextureDestRect;
};

}
}

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

	inline ci::gl::TextureRef getTexture() const { return mTexture; }
	inline void			setTexture(const ci::gl::TextureRef value, const bool resizeToTexture = true);
	
	//overloaded setup for applying custom texcoords
	void					setTexture(const ci::gl::TextureRef value, const std::vector<ci::vec2> coords, const bool resizeToTexture = true);
	
	//for updating the texCoords after initialization
	void					setTexCoords( const std::vector<ci::vec2> coords );
	std::vector<ci::vec2>	getTexCoords() { return mTexCoords; }

	//! Defaults to getDefaultScaleMode()
	inline ScaleMode	getScaleMode() const { return mScaleMode; }
	inline void			setScaleMode(const ScaleMode scaleMode) { mScaleMode = scaleMode; invalidate(false, true); }

	//! Defaults to STRETCH
	static ScaleMode	getDefaultScaleMode() { return sDefaultScaleMode; }
	static void			setDefaultScaleMode(const ScaleMode scaleMode) { sDefaultScaleMode = scaleMode; }

	//! Defaults to false. Can be set independently of the texture's top-down setting in case you have less control over that.
	void  setTopDown(const bool value)	{ mTopDown = value; }
	bool  getTopDown() const			{ return mTopDown; }

private:

	void draw() override;
	void validateContent() override;
	
	static ScaleMode	sDefaultScaleMode;

	ci::gl::BatchRef	mBatch;
	ci::gl::TextureRef	mTexture;
	ci::vec2			mTextureScale;
	ci::vec2			mTextureSize;
	ScaleMode			mScaleMode;
	bool				mTopDown;

	//normalized texture coordinates, clockwise starting at upper left
	std::vector<ci::vec2> mTexCoords;
	std::vector<ci::vec2> mUserTexCoords; 
	std::vector<ci::vec2> mDefaultTexCoords;
	bool				mHasCustomTexCoords;
	bool				mTexCoordsAreDirty;

};

}
}

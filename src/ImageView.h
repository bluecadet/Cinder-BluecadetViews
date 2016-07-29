//+---------------------------------------------------------------------------
//  Bluecadet Interactive 2016
//  Contents: 
//  Comments: 
//----------------------------------------------------------------------------
#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseView.h"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class ImageView> ImageViewRef;

class ImageView : public BaseView {

public:
	ImageView();
	~ImageView();

	virtual void reset();
	virtual void stopAnimation();

	virtual void setup(const std::string& fileName, const ci::vec2& size = ci::vec2(0));
	virtual void setup(const ci::gl::TextureRef texture, const ci::vec2& size = ci::vec2(0));

	virtual void setSize(const ci::vec2& size) override;

	inline void setImage(const std::string& fileName);
	inline void setImage(const ci::gl::TextureRef texture);

	inline void setTexture(const ci::gl::TextureRef value);
	inline ci::gl::TextureRef getTexture() const { return mTexture; }

	inline bool getAutoSizeToTexture() const { return mAutoSizeToTexture; }
	inline void setAutoSizeToTexture(const bool value) { mAutoSizeToTexture = value; }

private:

	virtual void draw() override;

	ci::gl::TextureRef	mTexture;

	bool				mAutoSizeToTexture;
	ci::Rectf			mDrawingDestRect;
	ci::Area			mDrawingArea;
};

}
}
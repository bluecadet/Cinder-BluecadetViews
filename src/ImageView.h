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

	virtual void setup(const std::string &fileName, const ci::vec2 &size = ci::vec2(0));
	virtual void setup(const ci::gl::TextureRef texture, const ci::vec2 &size = ci::vec2(0));

	void animateOn(float alpha, float aniDur, float aniDelay);
	void animateOff(float alpha, float aniDur, float aniDelay);

private:

	virtual void draw() override;

	// Properties
	ci::gl::TextureRef	mTexture;

	ci::Rectf				mDrawingDestRect;
	ci::Area				mDrawingArea;
};

}
}
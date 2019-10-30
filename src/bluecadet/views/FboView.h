#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseView.h"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class FboView> FboViewRef;

class FboView : public BaseView {

public:

	FboView();
	virtual ~FboView();

	//! Same as setSize(). Shorthand to get a rectangle with a size set up. Will create a new fbo. 
	void			setup(const ci::ivec2 & size, const float resolution = 1.0f);

	//! Will create a new fbo.
	void			setSize(const ci::vec2 & size) override;

	//! Will re-create the current fbo on the next draw call and marks the content to be re-drawn.
	virtual void	invalidateFbo() { mFbo = nullptr; invalidate(false, true); }

	//! Will create a new fbo with this format.
	virtual void	setFboFormat(const ci::gl::Fbo::Format & format) { mFboFormat = format; invalidateFbo(); }
	virtual ci::gl::Fbo::Format getFboFormat() const { return mFboFormat; }
	
	//! Force redraw fbo on each frame.
	bool			getForceRedraw() const { return mForceRedraw; }
	void			setForceRedraw(const bool value) { mForceRedraw = value; }

	bool			hasInvalidContent() const override { return mForceRedraw || BaseView::hasInvalidContent(); };

	//! Listens for contentUpdated events. This fires with it's own or a child view's size, scale, rotation, transform or position are updated.
	void			handleEvent(ViewEvent& event) override;

	//! Returns the fbo for this view. Be careful with this method since
	//!  - The result might be nullptr if the FBO hasn't been initialized (e.g. if setup was called).
	//!  - A new FBO might be recreated if the view's size is changed, so don't retain this FBO anywhere else.
	ci::gl::FboRef			getFbo() const { return mFbo; }

	//! Returns the FBO's color texture if the FBO is initialized. Otherwise returns nullptr.
	ci::gl::TextureRef		getTexture() const { return mFbo ? mFbo->getColorTexture() : nullptr; }

	//! When this value is set to true, this view will draw the color texture to the current frame buffer.
	//! If it's set to false, drawing this view will only draw its contents to the FBO, but not to screen.
	//! Defaults to true.
	void			setDrawsToScreen(const bool value) { mDrawsToScreen = value; }
	bool			getDrawsToScreen() const { return mDrawsToScreen; }

	//! Scale value that gets applied to the size of the fbo. All views are scaled by the same value, so they appear proportionally the same size.
	float			getResolution() const { return mResolution; }
	void			setFboScale(const float value);

	//! Sets the color to be used when clearing the fbo before rendering content
	void			setClearColor(const ci::ColorA & clearColor) { if (mClearColor != clearColor) { mClearColor = clearColor; invalidate(false, true); } }
	ci::ColorA		getClearColor() const { return mClearColor; }

protected:

	virtual ci::gl::FboRef	createFbo(const ci::ivec2 & size, const ci::gl::Fbo::Format & format);

	void			validateFbo();

	//! Redraw the fbo children
	inline void		validateContent() override;

	//! Draw content within this view
	void			draw() override;

	//! Re-draws this view's content to the FBO if necessary. Overrides BaseView, only called during validateContent for fbo view.
	void			drawChildren(const ci::ColorA& parentTint) override {};

	bool					mForceRedraw;
	bool					mDrawsToScreen;
	float					mResolution;

	ci::gl::Fbo::Format		mFboFormat;
	ci::gl::FboRef			mFbo; //! Careful, if fbo is invalidated this could be NULL!

	ci::ColorA				mClearColor;

};

}
}
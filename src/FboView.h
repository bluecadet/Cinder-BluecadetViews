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

	//! Shorthand to get a rectangle with a size set up. Will create a new fbo.
	void			setup(const ci::ivec2 & size);

	//! Will create a new fbo.
	void			setSize(const ci::vec2 & size) override;

	//! Will re-create the current fbo on the next draw call and marks the content to be re-drawn.
	virtual void	invalidateFbo() { mFbo = nullptr; mHasInvalidContent = true; }

	//! Will trigger the FBO to re-draw its content before the next draw call
	virtual void	invalidateContent() { mHasInvalidContent = true; }

	//! Will create a new fbo with this format.
	virtual void	setFboFormat(const ci::gl::Fbo::Format & format) { mFboFormat = format; invalidateFbo(); }
	virtual ci::gl::Fbo::Format getFboFormat() const { return mFboFormat; }
	
	//! Force redraw fbo on each frame.
	bool			getForceRedraw() const { return mForceRedraw; }
	void			setForceRedraw(const bool value) { mForceRedraw = value; }

	//! Listens for contentUpdated events. This fires with it's own or a child view's size, scale, rotation, transform or position are updated.
	virtual void	handleEvent(const Event& event) override;

protected:

	virtual ci::gl::FboRef	createFbo(const ci::ivec2 & size, const ci::gl::Fbo::Format & format);

	void			validateFbo();
	//! Redraw the fbo children
	void			validateContent();
	void			draw() override;
	//! Re-draws this view's content to the FBO if necessary.
	void			drawChildren(const ci::ColorA& parentTint) override;

	bool					mForceRedraw;
	bool					mHasInvalidContent;

	ci::gl::Fbo::Format		mFboFormat;
	ci::gl::FboRef			mFbo; //! Careful, if fbo is invalidated this could be NULL!

};

}
}
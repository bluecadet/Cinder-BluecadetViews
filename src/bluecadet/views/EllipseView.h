#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseView.h"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class EllipseView> EllipseViewRef;

class EllipseView : public BaseView {

public:

	//! Draws an ellipse with size extending around 0,0
	EllipseView();
	virtual ~EllipseView();

	//! Shorthand to get a circle with a background color set up
	void setup(const float radius, const ci::ColorA backgroundColor = ci::ColorA(), const float smoothness = 1.0f);

	//! Shorthand to get an ellipse with a background color set up
	void setup(const ci::vec2& size, const ci::ColorA backgroundColor = ci::ColorA(), const float smoothness = 1.0f);

	//! Shorthand for calling setSize(vec2(2.0f * radius))
	void setRadius(const float radius);

	//! Ellipse bounds extend -radius to +radius both for x and y
	ci::Rectf getBounds(const bool scaled) override {
		ci::Rectf bounds = BaseView::getBounds(scaled);
		bounds.offset(-0.5f * bounds.getSize());
		return bounds;
	}

	//! The smoothness of the ellipse's edge
	ci::Anim<float> &	getSmoothness() { return mSmoothness; }
	void				setSmoothness(const float value) { mSmoothness = value; invalidate(false, true); }

protected:
	virtual void draw() override;

	static ci::gl::BatchRef		getSharedEllipseBatch();
	static ci::gl::GlslProgRef	getSharedEllipseProg();

	ci::Anim<float> mSmoothness;

};

}
}
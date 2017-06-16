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

	//! The smoothness of the ellipse's edge
	inline float	getSmoothness() const { return mSmoothness; }
	inline void		setSmoothness(const float value) { mSmoothness = value; invalidate(false, true); }

protected:
	virtual void draw() override;
	virtual void debugDrawOutline() override;

	static ci::gl::BatchRef		getSharedEllipseBatch();
	static ci::gl::GlslProgRef	getSharedEllipseProg();

	float mSmoothness;

};

}
}
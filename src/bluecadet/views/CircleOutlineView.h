#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseView.h"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class CircleOutlineView> CircleOutlineViewRef;

class CircleOutlineView : public BaseView {

public:

	//! Draws an ellipse with size extending around 0,0
	CircleOutlineView();
	virtual ~CircleOutlineView();

	//! Shorthand to get a circle with a background color set up
	void setup(const float radius, const ci::ColorA backgroundColor = ci::ColorA(), const float borderThickeness = 1.0f, const float smoothness = 1.0f);

	//! Shorthand to get an ellipse with a background color set up
	void setup(const ci::vec2& size, const ci::ColorA backgroundColor = ci::ColorA(), const float borderThickeness = 1.0f, const float smoothness = 1.0f);

	//! Shorthand for calling setSize(vec2(2.0f * radius))
	void setRadius(const float radius);

	//! The smoothness of the ellipse's edge
	float	getSmoothness() const { return mSmoothness; }
	void	setSmoothness(const float value) { mSmoothness = value; invalidate(ValidationFlags::CONTENT); }

	float	getBorderThickness() const { return mBorderThickness; }
	void	setBorderThickness(const float value) { mBorderThickness = value; invalidate(ValidationFlags::CONTENT); }

protected:
	void draw() override;
	void debugDrawOutline() override;

	static ci::gl::BatchRef		getSharedEllipseBatch();
	static ci::gl::GlslProgRef	getSharedEllipseProg();

	float mSmoothness, mBorderThickness;

};

}
}
#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseView.h"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class StrokedCircleView> StrokedCircleViewRef;

class StrokedCircleView : public BaseView {

public:

	//! Draws an ellipse with size extending around 0,0
	StrokedCircleView();
	virtual ~StrokedCircleView();

	//! Shorthand to get a circle with a background color set up
	void setup(float radius, ci::ColorA backgroundColor = ci::ColorA(), float strokeWidth = 1.0f, float smoothness = 1.0f);

	//! Shorthand to get an ellipse with a background color set up
	void setup(ci::vec2 size, ci::ColorA backgroundColor = ci::ColorA(), float strokeWidth = 1.0f, float smoothness = 1.0f);

	//! Shorthand for calling setSize(vec2(2.0f * radius))
	void setRadius(const float radius);

	//! The smoothness of the ellipse's edge
	inline ci::Anim<float> &	getSmoothness() { return mSmoothness; }
	inline void					setSmoothness(float value) { mSmoothness = value; invalidate(false, true); }

	inline ci::Anim<float> &	getStrokeWidth() { return mStrokeWidth; }
	inline void					setStrokeWidth(float value) { mStrokeWidth = value; invalidate(false, true); }

protected:
	virtual void draw() override;
	virtual void debugDrawOutline() override;

	static ci::gl::BatchRef		getSharedBatch();
	static ci::gl::GlslProgRef	getSharedProg();

	ci::Anim<float> mSmoothness;
	ci::Anim<float> mStrokeWidth;

};

}
}
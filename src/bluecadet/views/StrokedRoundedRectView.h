#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseView.h"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class StrokedRoundedRectView> StrokedRoundedRectViewRef;

class StrokedRoundedRectView : public BaseView {

public:

	//! Draws an StrokedRoundedRect with size extending around 0,0
	StrokedRoundedRectView();
	virtual ~StrokedRoundedRectView();

	//! The color of the stroke
	inline void						setStrokeColor(const ci::ColorA & value) { mStrokeColor = value; }
	inline ci::Anim<ci::ColorA> &	getStrokeColor() { return mStrokeColor; }
	
	//! The width of the stroke (measured inwards)
	inline void						setStrokeWidth(float value) { mStrokeWidth = value; }
	inline ci::Anim<float> &		getStrokeWidth() { return mStrokeWidth; }

	//! The smoothness of the StrokedRoundedRect's edge
	inline ci::Anim<float> &		getSmoothness() { return mSmoothness; }
	inline void						setSmoothness(float value) { mSmoothness = value; invalidate(false, true); }

	//! The radius of all corners. Defaults to 0.
	inline ci::Anim<float> &		getCornerRadius() { return mCornerRadius; }
	inline void						setCornerRadius(const float value) { mCornerRadius = value; invalidate(false, true); }

	//! Cancels StrokedRoundedRectView animations in addition to BaseView animations.
	void cancelAnimations() override;

protected:
	void draw() override;

	static ci::gl::BatchRef		getSharedBatch();
	static ci::gl::GlslProgRef	getSharedProg();

	ci::Anim<ci::ColorA> mStrokeColor = ci::ColorA::white();
	ci::Anim<float> mSmoothness = 1.0f;
	ci::Anim<float> mStrokeWidth = 1.0f;
	ci::Anim<float> mCornerRadius = 0.0f;

};

}
}

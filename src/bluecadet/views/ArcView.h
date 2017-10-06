#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseView.h"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class ArcView> ArcViewRef;

class ArcView : public BaseView {

public:

	//! Draws an arc with size extending around 0,0
	ArcView();
	virtual ~ArcView();

	//! The outer radius of the arc (in px). Calls setSize() internally to change the size.
	inline void setOuterRadius(float radius)	{ mOuterRadius = radius; setSize(ci::vec2(2.0f * radius)); }
	inline ci::Anim<float> & getOuterRadius()	{ return mOuterRadius; invalidate(false, true); };

	//! The inner radius of the arc (in px)
	inline void setInnerRadius(float value)		{ mInnerRadius = value; }
	inline ci::Anim<float> & getInnerRadius()	{ return mInnerRadius; invalidate(false, true); }

	inline ci::Anim<float> & getStartAngle() 	{ return mStartAngle; }
	inline void setStartAngle(float value)		{ mStartAngle = value; invalidate(false, true); }

	inline ci::Anim<float> & getEndAngle() 		{ return mEndAngle; }
	inline void setEndAngle(float value)		{ mEndAngle = value; invalidate(false, true); }

	//! The smoothness of the arc's edge
	inline ci::Anim<float> & 	getSmoothness()	{ return mSmoothness; }
	inline void					setSmoothness(float value) { mSmoothness = value; invalidate(false, true); }

	//! Cancels ArcView animations in addition to BaseView animations.
	void cancelAnimations() override;

protected:
	void update(const double deltaTime) override;
	void draw() override;
	void debugDrawOutline() override;

	static ci::gl::BatchRef		getSharedBatch();
	static ci::gl::GlslProgRef	getSharedProg();

	ci::Anim<float> mSmoothness = 1.0f;
	ci::Anim<float> mInnerRadius = 0.0f;
	ci::Anim<float> mOuterRadius = 0.0f;
	ci::Anim<float> mStartAngle = 0.0f;
	ci::Anim<float> mEndAngle = glm::two_pi<float>();

};

}
}

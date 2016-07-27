#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseView.h"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class LineView> LineViewRef;

class LineView : public BaseView {

public:
	LineView();
	virtual ~LineView();

	//! The point that the line is drawn to. End point defaults to 0,0 (the start point is always at 0,0).
	const inline ci::vec2 getEndPoint()					{ return getSize(); }
	void inline setEndPoint(const ci::vec2& value)		{ setSize(value); }

	//! The thickness of the line. Defaults to 1.0f.
	inline ci::Anim<float> getLineWidth() const			{ return mLineWidth; }
	inline void setLineWidth(const float value)			{ mLineWidth = value; }

	//! The color of the line. Defaults to fully opaque white.
	inline const ci::Anim<ci::ColorA>& getLineColor() const	{ return mLineColor; }
	inline void setLineColor(const ci::ColorA& value)		{ mLineColor = value; }

protected:
	virtual void draw() override;

	ci::Anim<float>			mLineWidth;
	ci::Anim<ci::ColorA>	mLineColor;
};

}
}
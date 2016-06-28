//+---------------------------------------------------------------------------
//  Bluecadet Interactive 2016
//  Contents: 
//  Comments: 
//----------------------------------------------------------------------------
#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Timeline.h"

#include "BaseView.h"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class ShapeView> ShapeViewRef;

class ShapeView : public BaseView {

public:
	ShapeView();
	~ShapeView();

	void reset();

	void createLine(const ci::vec2 &startPos, const ci::vec2 &endPos, float lineWidth);
	void createSolidCircle(const ci::vec2 &pos, float radius);

	void animateOn(float alpha, float aniDur, float aniDelay);
	void animateOff(float alpha, float aniDur, float aniDelay);

private:

	virtual void draw() override;


	// Properties
	ci::gl::BatchRef	mLine;
	float				mLineWidth;

	ci::gl::BatchRef	mSolidCircle;

};

}
}
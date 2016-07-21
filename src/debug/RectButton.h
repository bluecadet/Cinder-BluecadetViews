//+---------------------------------------------------------------------------
//  Bluecadet Interactive 2016
//	Developers: Benjamin Bojko
//  Contents: 
//  Comments: 
//----------------------------------------------------------------------------

#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Timeline.h"

#include "../TouchView.h"
#include "RectView.h"

namespace bluecadet {
namespace views {
namespace debug {

typedef std::shared_ptr<class RectButton> RectButtonRef;

class RectButton : public TouchView {

public:

	RectButton();
	virtual ~RectButton();

	virtual void setup(const ci::vec2 size, const ci::ColorA& touchUpColor = ci::ColorA(1, 1, 0.5, 1.0), const ci::ColorA& touchDownColor = ci::ColorA(0.5, 1, 1, 1.0));
	virtual void update(const double deltaTime) override;

	ci::Anim<ci::ColorA> mTouchUpColor;
	ci::Anim<ci::ColorA> mTouchDownColor;

protected:
	virtual void draw() override;
	ci::gl::BatchRef mRectBatch;
};

}
}
}

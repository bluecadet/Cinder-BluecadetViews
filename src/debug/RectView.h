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

#include "../BaseView.h"

namespace bluecadet {
namespace views {
namespace debug {

typedef std::shared_ptr<class RectView> RectViewRef;

class RectView : public BaseView {

public:

	RectView(const ci::vec2& size, const ci::ColorA& tint = ci::ColorA(1, 0, 0, 0.5));
	virtual ~RectView();

protected:
	virtual void draw() override;
	ci::gl::BatchRef mRectBatch;
};

}
}
}
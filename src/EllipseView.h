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
	EllipseView();
	virtual ~EllipseView();

protected:
	virtual void draw() override;

	static ci::gl::BatchRef		getSharedEllipseBatch();
	static ci::gl::GlslProgRef	getSharedEllipseProg();


};

}
}
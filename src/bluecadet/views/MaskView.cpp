#include "MaskView.h"

#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

MaskView::MaskView() {
	static bool checkedForStencil = false;
	if (!checkedForStencil && getWindow()) {
		if (auto rendererGl = dynamic_pointer_cast<RendererGl>(getWindow()->getRenderer())) {
			if (!rendererGl->getOptions().getStencil()) {
				CI_LOG_E("MaskView requires your RendererGl settings to enable a stencil buffer. Set stencil(true) when creating your RendererGl in your main cpp.");
			}
		}
		checkedForStencil = true;
	}
}

MaskView::~MaskView() {
}

void MaskView::draw() {
	if (!mMask) {
		BaseView::draw();
		BaseView::drawChildren(getDrawColor());
		return;
	}

	// stencil code based on https://github.com/cinder/Cinder/blob/master/samples/_opengl/StencilReflection/src/StencilReflectionApp.cpp

	// enable stencil test to be able to give the stencil buffers values
	gl::ScopedState scopeStencil(GL_STENCIL_TEST, GL_TRUE);
	gl::stencilFunc(GL_ALWAYS, 1, 0xFF);
	gl::stencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	gl::stencilMask(0xFF);
	gl::depthMask(GL_FALSE);
	gl::clear(GL_STENCIL_BUFFER_BIT);

	// set all color channels to false to prevent mask from drawing to screen
	gl::colorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	// draw the stencil here since it's not actually a child
	mMask->drawScene(ColorA::black());

	// re-enable all color masks to draw content
	gl::colorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	// now tell the stencil what type of stenciling it has
	gl::stencilFunc(getStencilFuncEnum(), 1, 0xFF);
	gl::stencilMask(0x00);
	gl::depthMask(GL_TRUE);

	// reset to original draw color
	gl::color(getDrawColor());

	// draw into that stenciled area
	BaseView::draw();
	BaseView::drawChildren(getDrawColor());
}

void MaskView::updateScene(double deltaTime) {
	BaseView::updateScene(deltaTime);

	if (mMask) {
		// Update the mask here since it's not actually a child
		mMask->updateScene(deltaTime);
	}
}

inline GLenum MaskView::getStencilFuncEnum() const {
	return mMaskType == MaskType::REVEAL ? GL_EQUAL : GL_NOTEQUAL;
}

}
}

#include "MaskView.h"

#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

uint8_t MaskView::sStencilIndex = 0u;

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
	// nested stencil code based on https://stackoverflow.com/questions/13742556/best-approach-to-draw-clipped-ui-elements-in-opengl

	gl::ScopedState scopeStencil(GL_STENCIL_TEST, GL_TRUE);
	pushStencilState();

	++sStencilIndex;

	if (sStencilIndex == sMaxNestedStencils || sStencilIndex == 1) {
		if (sStencilIndex != 1) {
			CI_LOG_W("Max amount of nested MaskViews reached. The maxmimum is " + to_string(sMaxNestedStencils));
			sStencilIndex = 1;
		}
		gl::clear(GL_STENCIL_BUFFER_BIT);
	}

	// draw mask to stencil
	enableStencilDrawing(sStencilIndex, GL_INCR);
	mMask->drawScene(ColorA::black());

	// draw views to screen
	enableScreenDrawing(sStencilIndex, getStencilFuncEnum());
	BaseView::draw();
	BaseView::drawChildren(getDrawColor());

	// erase mask from stencil
	enableStencilDrawing(sStencilIndex, GL_DECR);
	mMask->drawScene(ColorA::black());

	// restore state
	if (--sStencilIndex == 0) {
		gl::clear(GL_STENCIL_BUFFER_BIT);
	}

	enableScreenDrawing(sStencilIndex, getStencilFuncEnum());

	popStencilState();
}

void MaskView::updateScene(const FrameInfo & info) {
	BaseView::updateScene(info);

	if (mMask) {
		// Update the mask here since it's not actually a child
		mMask->updateScene(info);
	}
}

inline void MaskView::pushStencilState() {
	glGetIntegerv(GL_STENCIL_FUNC, &mPushedStencilFunc);
	glGetIntegerv(GL_STENCIL_REF, &mPushedStencilRef);
	glGetIntegerv(GL_STENCIL_VALUE_MASK, &mPushedStencilMask);
}

inline void MaskView::popStencilState() {
	gl::stencilFunc(mPushedStencilFunc, mPushedStencilRef, mPushedStencilMask);
	mPushedStencilFunc = 0;
	mPushedStencilRef = 0;
	mPushedStencilMask = 0;
}

inline void MaskView::enableStencilDrawing(uint8_t index, GLenum stencilOp) {
	// enable stencil test to be able to give the stencil buffers values
	gl::stencilFunc(GL_ALWAYS, index, index);
	gl::stencilOp(stencilOp, stencilOp, stencilOp);

	// disable drawing to screen
	gl::colorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	gl::depthMask(GL_FALSE);
}

inline void MaskView::enableScreenDrawing(uint8_t index, GLenum stencilFunc) {
	// now tell the stencil what type of stenciling it has
	gl::stencilFunc(stencilFunc, index, index);
	gl::stencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	// enable drawing to screen
	gl::colorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	gl::depthMask(GL_TRUE);

	// reset to original draw color
	gl::color(getDrawColor());
}

inline GLenum MaskView::getStencilFuncEnum() const {
	return mMaskType == MaskType::REVEAL ? GL_LEQUAL : GL_GREATER;
}

}
}

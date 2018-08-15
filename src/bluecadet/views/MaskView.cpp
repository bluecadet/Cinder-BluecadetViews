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

	{
		// enable/increment stencil
		++sStencilIndex;

		if (sStencilIndex == sMaxNestedStencils) {
			CI_LOG_W("Max amount of nested MaskViews reached. The maxmimum is " + to_string(sMaxNestedStencils));
			sStencilIndex = 1;
			gl::clear(GL_STENCIL_BUFFER_BIT);
		}

		// enable stencil test to be able to give the stencil buffers values
		gl::stencilFunc(GL_ALWAYS, sStencilIndex, sStencilIndex);
		gl::stencilOp(GL_INCR, GL_INCR, GL_INCR);
		gl::depthMask(GL_FALSE);

		// set all color channels to false to prevent mask from drawing to screen
		gl::colorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		// draw the stencil here since it's not actually a child
		mMask->drawScene(ColorA::black());
	}

	{
		// re-enable all color masks to draw content
		gl::colorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		// now tell the stencil what type of stenciling it has
		gl::stencilFunc(getStencilFuncEnum(), sStencilIndex, sStencilIndex);
		gl::stencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		gl::depthMask(GL_TRUE);

		// reset to original draw color
		gl::color(getDrawColor());

		// draw into that stenciled area
		BaseView::draw();
		BaseView::drawChildren(getDrawColor());
	}

	{
		// disable stencil test to be able to give the stencil buffers values
		gl::stencilFunc(GL_ALWAYS, sStencilIndex, sStencilIndex);
		gl::stencilOp(GL_DECR, GL_DECR, GL_DECR);
		gl::depthMask(GL_FALSE);
		gl::colorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		// set all color channels to false to prevent mask from drawing to screen

		// draw the stencil here since it's not actually a child
		mMask->drawScene(ColorA::black());

		gl::colorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		gl::depthMask(GL_TRUE);

		--sStencilIndex;

		if (sStencilIndex == 0) {
			gl::clear(GL_STENCIL_BUFFER_BIT);
		}
	}
}

void MaskView::updateScene(double deltaTime) {
	BaseView::updateScene(deltaTime);

	if (mMask) {
		// Update the mask here since it's not actually a child
		mMask->updateScene(deltaTime);
	}

	sStencilIndex = 0; // Reset to 0 just in case view was removed/added while drawing
}

inline GLenum MaskView::getStencilFuncEnum() const {
	return mMaskType == MaskType::REVEAL ? GL_LEQUAL : GL_GREATER;
}

}
}

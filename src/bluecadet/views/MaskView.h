#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "bluecadet/views/BaseView.h"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class MaskView> MaskViewRef;

//! A view that can use any other type of view as a mask using
//! the OpenGL stencil buffer and the mask's alpha channel.
class MaskView : public bluecadet::views::BaseView {

public:

	enum class MaskType {
		REVEAL,	//! Pixels from the mask will reveal content pixels
		HIDE	//! Pixels from the mask will hide content pixels
	};

	MaskView();
	~MaskView();

	//! The view used to mask this view's content and children.
	//! The mask will be drawn and updated just like any other
	//! view and the resulting alpha channel will be used as a
	//! GL stencil mask. If no mask is defined, this view's
	//! contents will be drawn unmasked.
	//! IMPORTANT: Since we use GL stencil masking, any pixel
	//! output from content views such as semi-transparent or
	//! 0-alpha pixels result in 100% masked pixels. Use the
	//! `discard` keyword to unmask any pixels.
	void setMask(const BaseViewRef value)	{ mMask = value; }

	//! The mask view or nullptr.
	BaseViewRef getMask() const				{ return mMask; }

	//! The type of mask to use.
	void		setMaskType(MaskType value)	{ mMaskType = value; }
	MaskType	getMaskType() const			{ return mMaskType; }

protected:

	//! Draws the mask and its children.
	void draw() override;

	//! Updates itself and makes sure the mask is updated.
	void updateScene(const FrameInfo & info) override;

	//! Do nothing (will be called by draw().
	void drawChildren(const ci::ColorA& parentTint) override {};

	inline void pushStencilState();
	inline void popStencilState();

	inline void enableStencilDrawing(uint8_t index, GLenum stencilOp);
	inline void enableScreenDrawing(uint8_t index, GLenum stencilFunc);

	//! Gets the stencil op for current type
	inline GLenum getStencilFuncEnum() const;

	//! Used to support nested stencils
	static uint8_t sStencilIndex;
	static const uint8_t sMaxNestedStencils = 0xFFu; // 8 bit stencil buffer

	BaseViewRef mMask = nullptr;
	MaskType mMaskType = MaskType::REVEAL;

	GLint mPushedStencilFunc = 0;
	GLint mPushedStencilMask = 0;
	GLint mPushedStencilRef = 0;

};

}
}

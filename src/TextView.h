//+---------------------------------------------------------------------------
//  Bluecadet Interactive 2016
//  Contents: 
//  Comments: 
//----------------------------------------------------------------------------
#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "Text.h"
#include "StyledTextLayout.h"
#include "StyleManager.h"

#include "BaseView.h"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class TextView> TextViewRef;

class TextView : public BaseView, public text::StyledTextLayout {

public:
	TextView();
	~TextView();

	//! Creates a TextViewRef instance. Convenience method that groups a few calls together.
	static TextViewRef create(const std::string& text = "", const std::string& styleKey = "", const bool parseText = true, const float maxWidth = -1.0f);
	static TextViewRef create(const std::wstring& text = L"", const std::string& styleKey = "", const bool parseText = true, const float maxWidth = -1.0f);

	//! Configures a TextView instance. Convenience method that groups a few calls together.
	void setup(const std::wstring& text = L"", const std::string& styleKey = "", const bool parseText = true, const float maxWidth = -1.0f);
	void setup(const std::string& text = "", const std::string& styleKey = "", const bool parseText = true, const float maxWidth = -1.0f);

	virtual void	reset() override;

	//! Use these methods for more granular rendering control. Textures will otherwise automatically be rendered if necessary when draw() is called.
	bool			getAutoRenderEnabled() const { return mAutoRenderEnabled; }
	void			setAutoRenderEnabled(const bool value) { mAutoRenderEnabled = value; }

	bool			needsToBeRendered(bool surfaceOnly = false) const;

	//! Renders content. If surfaceOnly is false this will render into a texture and has to be called on the main thread. Surfaces can be rendered on a worker thread.
	void			renderContent(bool surfaceOnly = false, bool alpha = true, bool premultiplied = false, bool force = false);
	void			resetRenderedContent();

	//! Enabling smooth scaling uses mipmapping. Does not affect existing texture. Default is true.
	void			setSmoothScalingEnabled(const bool value) { mSmoothScalingEnabled = value; }
	bool			getSmoothScalingEnabled() const { return mSmoothScalingEnabled; }

	//! Sets a fixed size for the text view. Any values below 0 will allow the text view to automatically expand in that direction.
	virtual inline void				setSize(const ci::vec2& size) override;

	//! Returns the actual size of the text including padding.
	//! In advanced use-cases this can differ from what was set in setSize(), e.g. if a special clip mode is set or size-trimming is enabled.
	virtual inline const ci::vec2	getSize() override;

protected:

	//! Will update the text texture if necessary.
	virtual void			willDraw() override;
	virtual void			draw() override;

	ci::gl::Texture::Format createTextureFormat(bool smoothScaling) const;
	virtual inline void		invalidate(const bool layout = true, const bool size = true) override;

	// Change visibility of these methods from public to protected since setSize()/getSize() should be used.
	virtual ci::vec2		getMaxSize() const override { return StyledTextLayout::getMaxSize(); };
	virtual void			setMaxSize(const ci::vec2& size) override { return StyledTextLayout::setMaxSize(size); };
	virtual float			getMaxWidth() const override { return StyledTextLayout::getMaxWidth(); };
	virtual void			setMaxWidth(const float value) override { return StyledTextLayout::setMaxWidth(value); };
	virtual float			getMaxHeight() const override { return StyledTextLayout::getMaxHeight(); };
	virtual void			setMaxHeight(const float value) override { return StyledTextLayout::setMaxHeight(value); };

	bool				mHasInvalidRenderedContent;
	bool				mSmoothScalingEnabled;
	bool				mAutoRenderEnabled;

	ci::Surface			mSurface;
	ci::gl::TextureRef	mTexture;

};

}
}
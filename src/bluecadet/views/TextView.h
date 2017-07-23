//+---------------------------------------------------------------------------
//  Bluecadet Interactive 2016
//  Contents: 
//  Comments: 
//----------------------------------------------------------------------------
#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#if !defined(CINDER_MSW)

#warning "TextView: Error: This class is only supported on Windows 7+"

#else

#include "bluecadet/text/Text.h"
#include "bluecadet/text/StyledTextLayout.h"
#include "bluecadet/text/StyleManager.h"

#include "BaseView.h"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class TextView> TextViewRef;

class TextView : public BaseView, public text::StyledTextLayout {

public:
	TextView();
	virtual ~TextView();

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

	//! Use premultiplied alpha or not. Defaults to false.
	bool			getPremultiplied() const { return mPremultiplied; }
	void			setPremultiplied(const bool value) { mPremultiplied = value; }

	//! Sets a fixed, max size for the text view. Any values below 0 will allow the text view to automatically expand in that direction.
	void			setSize(const ci::vec2& size) override;


protected:

	//! Will update the text texture if necessary.
	void				willDraw() override;
	void				draw() override;

	ci::gl::Texture::Format createTextureFormat(bool smoothScaling) const;
	void				validateContent() override;
	void				invalidate(const bool layout = true, const bool size = true) override;
	void				invalidate(const int flags) override;

	// Change visibility of these methods from public to protected since setSize()/getSize() should be used.
	void				setMaxSize(const ci::vec2& size) override { return StyledTextLayout::setMaxSize(size); };
	void				setMaxWidth(const float value) override { return StyledTextLayout::setMaxWidth(value); };
	void				setMaxHeight(const float value) override { return StyledTextLayout::setMaxHeight(value); };
	ci::vec2			getMaxSize() const override { return StyledTextLayout::getMaxSize(); };
	float				getMaxWidth() const override { return StyledTextLayout::getMaxWidth(); };
	float				getMaxHeight() const override { return StyledTextLayout::getMaxHeight(); };

	bool				mHasInvalidRenderedContent;
	bool				mSmoothScalingEnabled;
	bool				mAutoRenderEnabled;
	bool				mPremultiplied;

	ci::Surface			mSurface;
	ci::gl::TextureRef	mTexture;

};

}
}

#endif

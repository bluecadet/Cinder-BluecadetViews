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

	static const ci::gl::Texture::Format & getDefaultTextureFormat();	//! Default format used for rendering. Optimized for 100% scale. No mip-mapping.
	static const ci::gl::Texture::Format & getMipMapTextureFormat();	//! MipMapped format optimized for scaling, but with slightly lower quality at 100%.

	//! Configures a TextView instance. Convenience method that groups a few calls together.
	void		setup(const std::wstring& text = L"", const std::string& styleKey = "", const bool parseText = true, const float maxWidth = -1.0f);
	void		setup(const std::string& text = "", const std::string& styleKey = "", const bool parseText = true, const float maxWidth = -1.0f);

	void		reset() override;

	//! Use these methods for more granular rendering control. Textures will otherwise automatically be rendered if necessary when draw() is called.
	bool		getAutoRenderEnabled() const { return mAutoRenderEnabled; }
	void		setAutoRenderEnabled(const bool value) { mAutoRenderEnabled = value; }

	bool		needsToBeRendered(bool surfaceOnly = false) const;

	//! Renders content. If surfaceOnly is false this will render into a texture and has to be called on the main thread. Surfaces can be rendered on a worker thread.
	void		renderContent(bool surfaceOnly = false, bool alpha = true, bool premultiplied = false, bool force = false);
	void		resetRenderedContent();

	//! The format used for the texture. Setting this will force a re-render of the texture. Defaults to TextView::getDefaultTextureFormat().
	void		setTextureFormat(const ci::gl::Texture::Format value) { mTextureFormat = value; mHasInvalidRenderedContent = true; }
	ci::gl::Texture::Format  getTextureFormat() const { return mTextureFormat; }

	//! Use premultiplied alpha or not. Defaults to false.
	bool		getPremultiplied() const { return mPremultiplied; }
	void		setPremultiplied(const bool value) { mPremultiplied = value; mHasInvalidRenderedContent = true; }

	//! Sets a fixed size for the text view. Any values below 0 will allow the text view to automatically expand in that direction.
	inline void	setSize(const ci::vec2& size) override;
	inline void	setWidth(const float width) override;
	inline void	setHeight(const float height) override;

	//! Returns the actual size of the text including padding.
	//! In advanced use-cases this can differ from what was set in setSize(), e.g. if a special clip mode is set or size-trimming is enabled.
	inline const ci::vec2	getSize() override;

protected:

	//! Will update the text texture if necessary.
	void		willDraw() override;
	void		draw() override;

	inline void	invalidate(const bool layout = true, const bool size = true) override;

	// Change visibility of these methods from public to protected since setSize()/getSize() should be used.
	const ci::vec2 &	getMaxSize() const override { return StyledTextLayout::getMaxSize(); };
	void				setMaxSize(const ci::vec2& size) override { return StyledTextLayout::setMaxSize(size); };
	float				getMaxWidth() const override { return StyledTextLayout::getMaxWidth(); };
	void				setMaxWidth(const float value) override { return StyledTextLayout::setMaxWidth(value); };
	float				getMaxHeight() const override { return StyledTextLayout::getMaxHeight(); };
	void				setMaxHeight(const float value) override { return StyledTextLayout::setMaxHeight(value); };

	bool				mHasInvalidRenderedContent;
	bool				mAutoRenderEnabled;
	bool				mPremultiplied;

	ci::Surface			mSurface;
	ci::gl::TextureRef	mTexture;
	ci::gl::Texture::Format	mTextureFormat;

};

}
}

#endif

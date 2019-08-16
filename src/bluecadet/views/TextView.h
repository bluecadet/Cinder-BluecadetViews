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
	TextView(const ci::gl::Texture::Format & textureFormat = getDefaultTextureFormat());
	virtual ~TextView();

	static ci::gl::Texture::Format getDefaultTextureFormat();

	//! Configures a TextView instance. Convenience method that groups a few calls together.
	void setup(const std::wstring& text = L"", const std::string& styleKey = "", const bool parseText = true, const float maxWidth = -1.0f, const bluecadet::text::TokenParserMapRef customTokenParsers = nullptr);
	void setup(const std::string& text = "", const std::string& styleKey = "", const bool parseText = true, const float maxWidth = -1.0f, const bluecadet::text::TokenParserMapRef customTokenParsers = nullptr);

	void	reset() override;

	//! Use these methods for more granular rendering control. Textures will otherwise automatically be rendered if necessary when draw() is called.
	bool	getAutoRenderEnabled() const { return mAutoRenderEnabled; }
	void	setAutoRenderEnabled(const bool value) { mAutoRenderEnabled = value; }

	bool	needsToBeRendered(bool surfaceOnly = false) const;

	//! Use these methods to compensate for GDI+'s poor alpha blending.
	//! Since GDI+ can't properly blend semi-transparent text pixels with
	//! a colored, but transparent background, text will have a slightly
	//! gray outline when scaled since the text is interpolated with a
	//! transparent black background. Setting this value to true will
	//! result in semi-transparent pixels having their rgb values divided
	//! by their alpha to compensate for that behavior.
	bool	getDemultiplyEnabled() const { return mDemultiplyEnabled; }
	void	setDemultiplyEnabled(const bool value) { mDemultiplyEnabled = value; }


	//! Renders content. If surfaceOnly is false this will render into a texture and has to be called on the main thread. Surfaces can be rendered on a worker thread.
	void	renderContent(bool surfaceOnly = false, bool alpha = true, bool premultiplied = false, bool force = false);
	void	resetRenderedContent();
	void	setBlendMode(const BlendMode blendMode) override;

	//! Sets a fixed size for the text view. Any values below 0 will allow the text view to automatically expand in that direction.
	inline void		setSize(const ci::vec2& size) override;
	inline void		setWidth(const float width) override;
	inline void		setHeight(const float height) override;


	//! Returns the actual size of the text including padding.
	//! In advanced use-cases this can differ from what was set in setSize(), e.g. if a special clip mode is set or size-trimming is enabled.
	inline const ci::vec2	getSize() override;

	//! The texture format used to create textures.
	void							setTextureFormat(const ci::gl::Texture::Format & value) { mTextureFormat = value; }
	const ci::gl::Texture::Format &	getTextureFormat() const { return mTextureFormat; }

protected:

	//! Will update the text texture if necessary.
	void			willDraw() override;
	void			draw() override;

	inline void		invalidate(const bool layout = true, const bool size = true) override;

	// Change visibility of these methods from public to protected since setSize()/getSize() should be used.
	const ci::vec2 &	getMaxSize() const override { return StyledTextLayout::getMaxSize(); };
	void				setMaxSize(const ci::vec2& size) override { return StyledTextLayout::setMaxSize(size); };
	float				getMaxWidth() const override { return StyledTextLayout::getMaxWidth(); };
	void				setMaxWidth(const float value) override { return StyledTextLayout::setMaxWidth(value); };
	float				getMaxHeight() const override { return StyledTextLayout::getMaxHeight(); };
	void				setMaxHeight(const float value) override { return StyledTextLayout::setMaxHeight(value); };

	bool				mHasInvalidRenderedContent = false;
	bool				mSmoothScalingEnabled = true;
	bool				mAutoRenderEnabled = true;
	bool				mDemultiplyEnabled = true;

	ci::Surface				mSurface;
	ci::gl::TextureRef		mTexture = nullptr;
	ci::gl::Texture::Format	mTextureFormat;

};

}
}

#endif

//+---------------------------------------------------------------------------
//  Bluecadet Interactive 2016
//  Contents: 
//  Comments: 
//----------------------------------------------------------------------------
#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "TouchView.h"
#include "ImageView.h"

typedef std::shared_ptr<class DetailImage> DetailImageRef;

class DetailImage : public bluecadet::views::TouchView {

public:
	DetailImage();
	~DetailImage();

	virtual void	setup(int id = 0);
	virtual void	update();

private:

	virtual void draw() override;

	//! Properties
	bluecadet::views::BaseViewRef	mBgImageContainer; //! Used to center the bg icon when we scale
	bluecadet::views::ImageViewRef	mBgImage;
	
	//! Callbacks
	void	handleBeginTouch(bluecadet::views::TouchViewRef button);
	void	handleEndTouch(bluecadet::views::TouchViewRef button);
	void	handleCancelTouch(bluecadet::views::TouchViewRef button);
};

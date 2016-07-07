//+---------------------------------------------------------------------------
//  Bluecadet Interactive 2016
//  Contents: 
//  Comments: 
//----------------------------------------------------------------------------
#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseView.h"
#include "MainContent/Heading.h"
#include "SubContent/DetailImage.h"

typedef std::shared_ptr<class ContentController> ContentControllerRef;

class ContentController : public bluecadet::views::BaseView {

public:
	ContentController();
	~ContentController();

	void setup();
	virtual void update(double deltaTime) override;

private:

	virtual void draw() override;

	//! Views
	HeadingRef		mHeading;
	DetailImageRef	mDetailImage;
};
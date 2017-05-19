//+---------------------------------------------------------------------------
//  Bluecadet Interactive 2016
//  Contents: 
//  Comments: 
//----------------------------------------------------------------------------
#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "bluecadet/views/BaseView.h"
#include "content/Header.h"
#include "content/DetailImage.h"

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
	HeaderRef mHeader;
	std::vector<DetailImageRef> mDetailImages;
};
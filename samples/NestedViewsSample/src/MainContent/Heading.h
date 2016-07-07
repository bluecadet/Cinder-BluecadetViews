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
#include "ShapeView.h"
#include "TextView.h"

typedef std::shared_ptr<class Heading> HeadingRef;

class Heading : public bluecadet::views::BaseView {

public:
	Heading();
	~Heading();

	virtual void	setup();
	virtual void	update();

private:

	virtual void draw() override;

	//! Properties
//	bluecadet::views::TextViewRef	mTitle; // SM :: Currently not working with latest Cinder - fixing
	bluecadet::views::ShapeViewRef	mLine1;
	bluecadet::views::ShapeViewRef	mLine2;
};

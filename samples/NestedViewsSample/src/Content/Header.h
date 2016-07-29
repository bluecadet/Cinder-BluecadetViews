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
#include "TextView.h"
#include "LineView.h"

typedef std::shared_ptr<class Header> HeaderRef;

class Header : public bluecadet::views::BaseView {

public:
	Header();
	~Header();

	virtual void	setup();
	virtual void	update();

private:

	virtual void draw() override;

	//! Properties
	bluecadet::views::TextViewRef	mTitle;
	bluecadet::views::LineViewRef	mUnderline;
};

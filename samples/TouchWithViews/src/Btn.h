//+---------------------------------------------------------------------------
//  Bluecadet Interactive 2016
//  Contents: 
//  Comments: 
//----------------------------------------------------------------------------
#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include <views/TouchView.h>
#include <views/EllipseView.h>

typedef std::shared_ptr<class Btn> BtnRef;

class Btn : public bluecadet::views::TouchView {

public:
	Btn();
	~Btn();

	virtual void setup();
	virtual void update(const double deltaTime) override;

private:

	// Properties
	bluecadet::views::EllipseViewRef	mBg;
	
	// Callbacks
	virtual void handleTouchBegan(const bluecadet::touch::TouchEvent& touchEvent) override;
	virtual void handleTouchEnded(const bluecadet::touch::TouchEvent& touchEvent) override;
};

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "TextView.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::views;
using namespace bluecadet::text;

class TextViewSampleApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

private:

	// Text
	TextViewRef	mTitle;
	int count;
};

void TextViewSampleApp::setup(){

	count = 0;

	mTitle = TextViewRef(new TextView());
	mTitle->setup("Sample Title " + toString(count));
	mTitle->setPosition(vec2(100, 300));
	// Since it's a view, we have access to properties like this (animation help)
	mTitle->setAlpha(0.75f);
	// You can set the color of the view, but this will not change the color of the text (because it's based on the style)
	mTitle->setColor(ColorA(1.0f, 0.0f, 0.0f, mTitle->getAlpha()));
}

void TextViewSampleApp::mouseDown( MouseEvent event ){

	// Increase count to be displayed
	count++;

	// Clear and reset the text
	mTitle->setText("Sample Title " + toString(count));

	if (count % 2)	mTitle->setScale(vec2(1.5));
	else			mTitle->setScale(vec2(1));
}

void TextViewSampleApp::update(){
}

void TextViewSampleApp::draw(){
	gl::clear( Color( .5, .5, 0 ) ); 

	mTitle->drawScene();
}

CINDER_APP( TextViewSampleApp, RendererGl )

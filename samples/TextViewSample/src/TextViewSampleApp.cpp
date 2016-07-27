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
	void mouseDown(MouseEvent event) override;
	void update() override;
	void draw() override;

private:
	TextViewRef	mTitle;
	int count;
};

void TextViewSampleApp::setup() {

	count = 0;

	mTitle = TextViewRef(new TextView());

	// This is what actually changes the text color; Gets multiplied with tint color
	mTitle->setMaxWidth(500.0f);
	mTitle->setBackgroundColor(Color(1, 0, 0));
	mTitle->setTextColor(Color(1.0f, 1.0f, 1.0f));

	// Change font size
	mTitle->setFontSize(64.0f);
	
	// All styles will be applied to text now
	mTitle->setTextAlign(TextAlign::Center);
	mTitle->setText("Sample Title " + toString(count));
}

void TextViewSampleApp::mouseDown(MouseEvent event) {

	// Increase count to be displayed
	count++;

	// Clear and reset the text
	mTitle->setText("Sample Title " + toString(count));

	vec2 padding = vec2(16.f, 16.f);

	// top left plus padding
	vec2 pos = vec2(0) + padding;
	
	if (count % 2) {
		// bottom right minus padding
		pos = vec2(getWindowSize()) - mTitle->getSize() - padding;
	}

	mTitle->getTimeline()->apply(&mTitle->getPosition(), pos, 0.33f, EaseOutElastic(1.1f, 0.75f));
}

void TextViewSampleApp::update() {
	mTitle->updateScene(0);
}

void TextViewSampleApp::draw() {
	gl::clear(Color(0.2f, 0.2f, 0.2f));
	mTitle->drawScene();
}

CINDER_APP(TextViewSampleApp, RendererGl)

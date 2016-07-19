#include "Header.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::views;

//==================================================
// Class Lifecycle
// 

Header::Header() : BaseView() {
}

Header::~Header(){
}

void Header::setup() {

	const ci::ColorA red = ColorA(1.0f, 0.0f, 0.0f, 1.0f);

	// Create title
	mTitle = TextViewRef(new TextView());
	mTitle->setTextColor(red);
	mTitle->setFontSize(40.0f, false);
	mTitle->setText(toString("Testing Title"));
	mTitle->setPosition(vec2(-mTitle->getWidth()*0.5f, 0.0f)); // Center title within header view horizontally

	// Create underline
	mUnderline = ShapeViewRef(new ShapeView());
	float lineThickness = 4.0f;
	float yPos = mTitle->getPosition().value().y + mTitle->getHeight() + 10.0f;
	const vec2 startPos = vec2(mTitle->getPosition().value().x, yPos);
	const vec2 endPos = vec2(startPos.x + mTitle->getWidth(), yPos);
	mUnderline->createLine(startPos, endPos, lineThickness);
	mUnderline->setTint(red);

	// Add views
	addChild(mTitle);
	addChild(mUnderline);
}

void Header::update() {
}

void Header::draw() {

}
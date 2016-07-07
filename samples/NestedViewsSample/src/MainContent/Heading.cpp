#include "Heading.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace bluecadet::views;

//==================================================
// Class Lifecycle
// 

Heading::Heading() : BaseView() {
}

Heading::~Heading(){
}

void Heading::setup() {

	float lineThickness = 6.0f;
	float lineWidth = 600.0f;
	float yPos = 100.0f;

	// Create line above text
	mLine1 = ShapeViewRef(new ShapeView());
	vec2 startPos = vec2(getWindowWidth()*.5f - lineWidth*0.5f, yPos);
	vec2 endPos = vec2(getWindowWidth()*.5f + lineWidth*0.5f, yPos);
	mLine1->createLine(startPos, endPos, lineThickness);
	mLine1->setColor(ColorA(1.0f, 0.0f, 0.0f, 1.0f));

	// Create line below text
	mLine2 = ShapeViewRef(new ShapeView());
	yPos += 100.0f;
	startPos = vec2(getWindowWidth()*.5f - lineWidth*0.5f, yPos);
	endPos = vec2(getWindowWidth()*.5f + lineWidth*0.5f, yPos);
	mLine2->createLine(startPos, endPos, lineThickness);
	mLine2->setColor(ColorA(1.0f, 0.0f, 0.0f, 1.0f));

	// Add views
	addChild(mLine1);
	addChild(mLine2);

	// Create title
	//mTitle = TextViewRef(new TextView());
	//mTitle->setup(toString("Testing Title"));
	// Position between lines
	
	//addChild(mTitle);

}

void Heading::update() {
}

void Heading::draw() {

}
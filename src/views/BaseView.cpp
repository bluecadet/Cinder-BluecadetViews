#include "BaseView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

//==================================================
// Lifecycle
// 

BaseView::BaseView() :
	mTransformOrigin(vec2(0.0f, 0.0f)),
	mPosition(vec2(0.0f, 0.0f)),
	mScale(vec2(1.0f, 1.0f)),
	mRotation(quat()),
	mTransform(mat4()),
	mGlobalTransform(mat4()),
	mHasInvalidTransforms(true),

	mHasInvalidContent(true),

	mTint(Color(1.0f, 1.0f, 1.0f)),
	mDrawColor(ColorA(1.0f, 1.0f, 1.0f, 1.0f)),
	mBackgroundColor(ColorA(0, 0, 0, 0)),

	mAlpha(1.0),
	mIsHidden(false),
	mShouldForceRedraw(false),

	mTimeline(Timeline::create()),

	mChildren(),
	mParent(nullptr),

	mSize(0, 0) {
}

BaseView::~BaseView() {
	mParent = nullptr;
}

void BaseView::reset() {
	mPosition = vec2(0.0f, 0.0f);
	mScale = vec2(1.0f, 1.0f);
	mRotation = quat();
	mTransform = mat4();
	mGlobalTransform = mat4();
	mHasInvalidTransforms = true;
	mHasInvalidContent = true;
	mTint = Color(1.0f, 1.0f, 1.0f);
	mDrawColor = ColorA(1.0f, 1.0f, 1.0f, 1.0f);
	mBackgroundColor = ColorA(0, 0, 0, 0);
	mSize = vec2(0, 0);
	mAlpha = 1.0;
}

//==================================================
// Parent/child relationships
// 

void BaseView::addChild(BaseViewRef child) {
	addChild(child, mChildren.size());
}

void BaseView::addChild(BaseViewRef child, size_t index) {
	if (!child) {
		cout << "Trying to add empty child; aborting" << endl;
		return;
	}

	if (child.get() == this) {
		cout << "Can't add self as child" << endl;
		return;
	}

	if (child.get() == mParent) {
		cout << "Can't add own parent as child" << endl;
		return;
	}

	if (child->mParent) {
		child->mParent->removeChild(child);
	}

	child->mParent = this;

	index = max((size_t)0, min(index, mChildren.size()));

	if (index == mChildren.size()) {
		mChildren.push_back(child);

	} else {
		auto it = mChildren.begin();
		std::advance(it, index);
		mChildren.insert(it, child);
	}

	child->didMoveToView(this);
}

void BaseView::removeChild(BaseViewRef child) {
	if (!child) {
		cout << "Trying to remove empty child; aborting" << endl;
		return;
	}

	if (child->mParent != this) {
		cout << "Can't remove node that's not a child; aborting" << endl;
		return;
	}

	child->willMoveFromView(this);
	child->mParent = nullptr;
	mChildren.remove(child);
}

void BaseView::removeChild(BaseView* childPtr) {
	auto childIt = getChildIt(childPtr);
	if (childIt == mChildren.end()) {
		cout << "Could not find child" << endl;
		return;
	}
	removeChild(childIt);
}

BaseViewList::iterator BaseView::removeChild(BaseViewList::iterator childIt) {
	(*childIt)->willMoveFromView(this);
	(*childIt)->mParent = nullptr;
	return mChildren.erase(childIt);
}

void BaseView::removeAllChildren() {
	for (BaseViewList::iterator it = mChildren.begin(); it != mChildren.end();) {
		it = removeChild(it);
	}
}

//==================================================
// Order and Sorting
//

int BaseView::getChildIndex(BaseViewRef child) {
	int index = 0;
	for (auto c : mChildren) {
		if (c == child)	return index;
		++index;
	}
	return -1;
}

void BaseView::moveToFront() {
	if (!mParent) {
		return;
	}
	mParent->moveChildToIndex(this, mParent->mChildren.size());
}

void BaseView::moveToBack() {
	if (!mParent) {
		return;
	}
	mParent->moveChildToIndex(this, 0);
}

void BaseView::moveChildToIndex(BaseView* childPtr, size_t index) {
	auto childIt = getChildIt(childPtr);
	if (childIt == mChildren.end()) {
		cout << "Could not find child" << endl;
		return;
	}
	moveChildToIndex(childIt, index);
}

void BaseView::moveChildToIndex(BaseViewRef child, size_t index) {
	auto childIt = getChildIt(child);
	if (childIt == mChildren.end()) {
		cout << "Could not find child" << endl;
		return;
	}
	moveChildToIndex(childIt, index);
}

void BaseView::moveChildToIndex(BaseViewList::iterator childIt, size_t index) {
	size_t currentIndex = std::distance(mChildren.begin(), childIt);
	index = max((size_t)0, min(mChildren.size() - 1, index));

	if (index > currentIndex) {
		// incr by 1 if we're moving the element beyond its old index
		// since splice removes the element before inserting again
		index += 1;
	}

	auto targetIt = mChildren.begin();
	std::advance(targetIt, index);

	mChildren.splice(targetIt, mChildren, childIt);
}


//==================================================
// Transformation
// 

void BaseView::setTransformOrigin(const vec2 & value, const bool compensateForOffset) {
	if (!compensateForOffset) {
		setTransformOrigin(value); return;
	}

	// an arbitrary point in our view. needs to be vec4 with w = 1 for mat4 mulitiplication
	static const vec4 center = vec4(0, 0, 0, 1);

	// save our point when transformed with the current transform origin
	const vec2 transformedCenterBefore = vec2(getTransform() * center);

	// set the new origin
	setTransformOrigin(value);

	// now see where the same point is in our view if we transform it with the new origin
	const vec2 transformedCenterAfter = vec2(getTransform() * center);

	// calculate the offset between new and old
	const vec2 centerOffset = transformedCenterBefore - transformedCenterAfter;

	// apply the offset so that our point visually stays at the same position
	setPosition(mPosition.value() + centerOffset);
}

//==================================================
// Main loop
// 

void BaseView::updateScene(const double deltaTime) {
	if (mTimeline && !mTimeline->empty()) {
		mTimeline->stepTo(timeline().getCurrentTime());
		invalidate();
	}

	if (mHasInvalidContent) {
		dispatchEvent(Event(Event::Type::UPDATED, this));
	}

	update(deltaTime);
	for (auto child : mChildren) {
		child->updateScene(deltaTime);
	}
}

void BaseView::update(const double deltaTime) {

}

void BaseView::drawScene(const ColorA& parentTint) {
	if (!mShouldForceRedraw && (mIsHidden || mAlpha <= 0.0f)) {
		return;
	}

	validateTransforms();
	validateContent();

	mDrawColor.r = mTint.value().r * parentTint.r;
	mDrawColor.g = mTint.value().g * parentTint.g;
	mDrawColor.b = mTint.value().b * parentTint.b;
	mDrawColor.a = mAlpha.value() * parentTint.a;

	{
		gl::ScopedModelMatrix scopedModelMatrix;
		gl::ScopedViewMatrix scopedViewMatrix;

		gl::multModelMatrix(mTransform);
		gl::color(mDrawColor);

		willDraw();
		draw();
		drawChildren(mDrawColor);
		didDraw();
	}
}

void BaseView::draw() {
	// override this method for custom drawing

	const auto& bgColor = mBackgroundColor.value();
	const auto size = getSize();

	if (size.x <= 0 && size.y <= 0 && bgColor.a <= 0) {
		return;
	}

	auto prog = getDefaultDrawProg();
	auto batch = getDefaultDrawBatch();

	prog->uniform("uSize", size);
	prog->uniform("uBackgroundColor", vec4(bgColor.r, bgColor.g, bgColor.b, bgColor.a));
	batch->draw();
}

//==================================================
// Animation
// 

void BaseView::resetAnimations() {
	if (mTimeline) {
		mTimeline->clear();
	}
	mPosition.stop();
	mRotation.stop();
	mScale.stop();
	mTint.stop();
	mAlpha.stop();
}

TimelineRef BaseView::getTimeline() {
	if (!mTimeline) {
		mTimeline = Timeline::create();
	}
	if (mTimeline) {
		mTimeline->stepTo(timeline().getCurrentTime());
	}
	return mTimeline;
}

CueRef BaseView::dispatchAfter(std::function<void()> fn, float delay) {
	return getTimeline()->add(fn, getTimeline()->getCurrentTime() + delay);
}

//==================================================
// Events
// 

void BaseView::dispatchEvent(Event& event) {
	if (!event.target) {
		event.target = this;
	} else {
		handleEvent(event);
	}
	event.currentTarget = this;
	if (mParent) {
		mParent->dispatchEvent(event);
	}
}

//EventSignal::slot_type BaseView::addEventCallback(EventCallback callback, const std::string type) {
//	return mEventSignal.connect([=](const Event & event) {
//		if (event.type == type) {
//			callback(event);
//		}
//	});
//}


//==================================================
// Drawing
// 

gl::GlslProgRef BaseView::getDefaultDrawProg() {
	static gl::GlslProgRef defaultProg = nullptr;

	if (!defaultProg) {
		defaultProg = gl::GlslProg::create(
			gl::GlslProg::Format().vertex(CI_GLSL(150,
				uniform vec2	uSize;
		uniform mat4	ciModelViewProjection;
		in vec4			ciPosition;
		in vec4			ciColor;
		out vec4		color;
		void main(void) {
			vec4 pos = vec4(ciPosition.x * uSize.x, ciPosition.y * uSize.y, 0.0f, 1.0f);
			gl_Position = ciModelViewProjection * pos;
			color = ciColor;
		}
		)).fragment(CI_GLSL(150,
			in vec4			color;
		out vec4		oColor;
		uniform vec4	uBackgroundColor;
		void main(void) {
			oColor = color * uBackgroundColor;
		}
		))
			);
		defaultProg->uniform("uSize", vec2(0, 0));
	}

	return defaultProg;
}

gl::BatchRef BaseView::getDefaultDrawBatch() {
	static gl::BatchRef defaultBatch = nullptr;
	if (!defaultBatch) {
		auto rect = geom::Rect().rect(Rectf(0, 0, 1, 1));
		defaultBatch = gl::Batch::create(rect, getDefaultDrawProg());
	}
	return defaultBatch;
}

}
}

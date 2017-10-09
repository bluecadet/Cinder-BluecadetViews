#include "BaseView.h"
#include <cinder/Log.h>

#ifdef _DEBUG
#include "cinder/Rand.h"
#endif

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

//==================================================
// Defaults
// 

bool BaseView::sEventPropagationEnabled = true;
bool BaseView::sContentInvalidationEnabled = true;
bool BaseView::sDebugDrawBounds = false;
bool BaseView::sDebugDrawInvisibleBounds = false;

//==================================================
// Lifecycle
// 

size_t BaseView::sNumInstances = 0;

BaseView::BaseView() :
	mTransformOrigin(vec2(0.0f)),
	mPosition(vec2(0.0f)),
	mScale(vec2(1.0f)),
	mRotation(quat()),
	mHasInvalidTransforms(true),
	mSize(0),
	mHasInvalidContent(true),

	mTint(Color::white()),
	mBackgroundColor(ColorA::zero()),
	mDrawColor(1.0f, 1.0f, 1.0f, 1.0f),

	mAlpha(1.0),
	mIsHidden(false),
	mShouldForceInvisibleDraw(false),
	mBlendMode(BlendMode::INHERIT),

	mShouldPropagateEvents(sEventPropagationEnabled),
	mShouldDispatchContentInvalidation(sContentInvalidationEnabled),

	mTimeline(Timeline::create()),
	mParent(nullptr),
	
	mViewId(sNumInstances++),
	mViewIdStr(to_string(mViewId))
	{
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
	mShouldForceInvisibleDraw = false;
	mShouldPropagateEvents = true;
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
		CI_LOG_W("Trying to add empty child; aborting");
		return;
	}

	if (child.get() == this) {
		CI_LOG_W("Can't add self as child");
		return;
	}

	if (child.get() == mParent) {
		CI_LOG_W("Can't add own parent as child");
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
		CI_LOG_W("Trying to remove empty child; aborting");
		return;
	}

	if (child->mParent != this) {
		CI_LOG_W("Can't remove node that's not a child; aborting");
		return;
	}

	child->willMoveFromView(this);
	child->mParent = nullptr;
	mChildren.remove(child);
}

void BaseView::removeChild(BaseView* childPtr) {
	auto childIt = getChildIt(childPtr);
	if (childIt == mChildren.end()) {
		CI_LOG_W("Could not find child");
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
		CI_LOG_W("Could not find child");
		return;
	}
	moveChildToIndex(childIt, index);
}

void BaseView::moveChildToIndex(BaseViewRef child, size_t index) {
	auto childIt = getChildIt(child);
	if (childIt == mChildren.end()) {
		CI_LOG_W("Could not find child");
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

	if (mHasInvalidContent && mShouldDispatchContentInvalidation) {
		dispatchEvent(ViewEvent(ViewEvent::Type::CONTENT_INVALIDATED, getSharedViewPtr()));
	}

	update(deltaTime);
	for (auto child : mChildren) {
		child->updateScene(deltaTime);
	}
}

void BaseView::drawScene(const ColorA& parentTint) {
	const bool shouldDraw = mShouldForceInvisibleDraw || (!mIsHidden && mAlpha > 0.0f);

	if (shouldDraw || (sDebugDrawBounds && sDebugDrawInvisibleBounds)) {
		validateTransforms();
		validateContent();

		mDrawColor.r = mTint.value().r * parentTint.r;
		mDrawColor.g = mTint.value().g * parentTint.g;
		mDrawColor.b = mTint.value().b * parentTint.b;
		mDrawColor.a = mAlpha.value() * parentTint.a;

		gl::ScopedModelMatrix scopedModelMatrix;
		gl::ScopedViewMatrix scopedViewMatrix;

		gl::multModelMatrix(mTransform);
		gl::color(mDrawColor);

		willDraw();

		switch (mBlendMode) {
			case BlendMode::INHERIT: {
				draw();
				drawChildren(mDrawColor);
				break;
			} case BlendMode::ALPHA: {
				gl::ScopedBlendAlpha scopedBlend;
				draw();
				drawChildren(mDrawColor);
				break;
			} case BlendMode::PREMULT: {
				gl::ScopedBlendPremult scopedBlend;
				draw();
				drawChildren(mDrawColor);
				break;
			} case BlendMode::ADD: {
				gl::ScopedBlend scopedBlend(GL_SRC_ALPHA, GL_ONE);
				draw();
				drawChildren(mDrawColor);
				break;
			} case BlendMode::MULTIPLY: {
				gl::ScopedBlend scopedBlend(GL_DST_COLOR, GL_ZERO);
				draw();
				drawChildren(mDrawColor);
				break;
			}
		}

		if (sDebugDrawBounds) {
			debugDrawOutline();
		}

		didDraw();
	}
}

void BaseView::draw() {
	// override this method for custom drawing
	const auto size = getSize();
	const auto & color = getBackgroundColor().value();

	if (size.x <= 0 && size.y <= 0 && color.a <= 0) {
		return;
	}

	auto prog = getDefaultDrawProg();
	auto batch = getDefaultDrawBatch();

	prog->uniform("uSize", size);
	prog->uniform("uBackgroundColor", vec4(color));
	batch->draw();
}

inline void BaseView::debugDrawOutline() {
	const float hue = (float)mViewId / (float)sNumInstances;
	const auto color = ColorAf(ci::hsvToRgb(vec3(hue, 1.0f, 1.0f)), 0.66f);
	gl::color(color);
	gl::drawStrokedRect(Rectf(vec2(0), getSize()));
}

//==================================================
// Animation
// 

void BaseView::cancelAnimations() {
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

void BaseView::dispatchEvent(ViewEvent & event) {
	if (!event.target) {
		event.target = getSharedViewPtr();
	} else {
		handleEvent(event);
	}

	mEventSignalsByType[event.type].emit(event);

	if (!event.shouldPropagate || !mShouldPropagateEvents) {
		// cut off propagation if required
		return;
	}

	event.currentTarget = getSharedViewPtr();

	if (mParent) {
		mParent->dispatchEvent(event);
	}
}

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

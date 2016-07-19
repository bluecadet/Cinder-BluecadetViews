#include "BaseView.h"

using namespace ci;
using namespace ci::app;
using namespace std;

//==================================================
// Lifecycle
// 
namespace bluecadet {
namespace views {

BaseView::BaseView() :
	mPosition(vec2(0.0f, 0.0f)),
	mScale(vec2(1.0f, 1.0f)),
	mRotation(quat()),
	mTransform(mat4()),
	mGlobalTransform(mat4()),
	mHasInvalidTransforms(true),

	mTint(Color(1.0f, 1.0f, 1.0f)),
	mTintA(ColorA(1.0f, 1.0f, 1.0f, 1.0f)),
	mAlpha(1.0),
	mIsHidden(false),
	mShouldForceRedraw(false),

	//mTimeline(nullptr),
	mTimeline(ci::Timeline::create()),

	mChildren(),
	mParent(nullptr)
{
}

BaseView::~BaseView() {
	mChildren.clear();
	mParent = nullptr;
}

void BaseView::reset() {
	mPosition = vec2(0.0f, 0.0f);
	mScale = vec2(1.0f, 1.0f);
	mRotation = quat();
	mTransform = mat4();
	mGlobalTransform = mat4();
	mHasInvalidTransforms = true;
	mTint = Color(1.0f, 1.0f, 1.0f);
	mTintA = ColorA(1.0f, 1.0f, 1.0f, 1.0f);
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

	}
	else {
		auto it = mChildren.begin();
		std::advance(it, index);
		mChildren.insert(it, child);
	}

	child->validateTransforms();
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
	child->validateTransforms();
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

void BaseView::didMoveToView(BaseView* parent) {
	// override this method
}

void BaseView::willMoveFromView(BaseView* parent) {
	// override this method
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
// Main loop
// 

void BaseView::updateScene(const double deltaTime) {
	if (mTimeline && !mTimeline->empty()) {
		mTimeline->stepTo(timeline().getCurrentTime());
	}
	update(deltaTime);
	for (auto child : mChildren) {
		child->updateScene(deltaTime);
	}
}

void BaseView::update(const double deltaTime) {

}

void BaseView::drawScene(const ci::ColorA& parentTint) {
	if (!mShouldForceRedraw && (mIsHidden || mAlpha <= 0.0f)) {
		return;
	}

	// recalculate transforms if marked as dirty or while animations are happening
	// this way children will also know that their parent transform has changed
	mHasInvalidTransforms = mHasInvalidTransforms || (mParent && mParent->mHasInvalidTransforms) ||
		!mPosition.isComplete() || !mScale.isComplete() || !mRotation.isComplete();

	if (mHasInvalidTransforms) {
		validateTransforms(false);
	}

	mTintA.r = mTint.value().r * parentTint.r;
	mTintA.g = mTint.value().g * parentTint.g;
	mTintA.b = mTint.value().b * parentTint.b;
	mTintA.a = mAlpha.value() * parentTint.a;

	{
		gl::ScopedModelMatrix scopedModelMatrix;
		gl::ScopedViewMatrix scopedViewMatrix;
		//gl::ScopedBlendAlpha scopedBlendAlpha; // bb: no real need for this at this point since we're not changing blend modes a lot

		gl::multModelMatrix(mTransform);
		gl::color(mTintA);

		willDraw();
		draw();
		drawChildren(mTintA);
		didDraw();
	}

	// clear dirty flag at end so that children know that things have changed
	mHasInvalidTransforms = false;
}

void BaseView::willDraw() {
	// override this method
}

void BaseView::draw() {
	// override this method
}

void BaseView::drawChildren(const ci::ColorA& parentTint) {
	for (auto child : mChildren) {
		child->drawScene(parentTint);
	}
}

void BaseView::didDraw() {
	// override this method
}

//==================================================
// Local/Global Transforms
// 

void BaseView::validateTransforms(const bool clearInvalidFlag) {
	mTransform = glm::translate(vec3(mPosition.value(), 0.0f));
	mTransform *= glm::scale(vec3(mScale.value(), 1.0f));
	mTransform *= glm::toMat4(mRotation.value());
	mGlobalTransform = mParent ? mParent->mGlobalTransform * mTransform : mTransform;
	if (clearInvalidFlag) {
		mHasInvalidTransforms = false;
	}
}

const ci::vec2 BaseView::convertLocalToGlobal(const ci::vec2& local) const {
	vec4 global = mGlobalTransform * vec4(local, 0, 1);
	return vec2(global.x, global.y);
}

const ci::vec2 BaseView::convertGlobalToLocal(const ci::vec2& global) const {
	vec4 local = glm::inverse(mGlobalTransform) * vec4(global, 0, 1);
	return vec2(local.x, local.y);
}

const ci::vec2 BaseView::getGlobalPosition() const {
	if (!mParent) {
		return mPosition;
	}
	return mParent->convertLocalToGlobal(mPosition);
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

ci::TimelineRef BaseView::getTimeline() {
	if (!mTimeline) {
		mTimeline = ci::Timeline::create();
	}
	if (mTimeline) {
		mTimeline->stepTo(timeline().getCurrentTime());
	}
	return mTimeline;
}

ci::CueRef BaseView::dispatchAfter(std::function<void()> fn, float delay) {
	return getTimeline()->add(fn, getTimeline()->getCurrentTime() + delay);
}

//==================================================
// Helpers
// 

BaseViewList::iterator BaseView::getChildIt(BaseViewRef child) {
	if (!child || child->mParent != this) {
		return mChildren.end();
	}
	return std::find(mChildren.begin(), mChildren.end(), child);
}

BaseViewList::iterator BaseView::getChildIt(BaseView* childPtr) {
	if (!childPtr || childPtr->mParent != this) {
		return mChildren.end();
	}
	return std::find_if(mChildren.begin(), mChildren.end(), [&](BaseViewRef child) {
		return child.get() == childPtr;
	});
}

}
}
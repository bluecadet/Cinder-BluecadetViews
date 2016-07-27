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
	mTransformOrigin(vec2(0.0f, 0.0f)),
	mPosition(vec2(0.0f, 0.0f)),
	mScale(vec2(1.0f, 1.0f)),
	mRotation(quat()),
	mTransform(mat4()),
	mGlobalTransform(mat4()),
	mHasInvalidTransforms(true),
	mHasInvalidUniforms(true),

	mTint(Color(1.0f, 1.0f, 1.0f)),
	mDrawColor(ColorA(1.0f, 1.0f, 1.0f, 1.0f)),
	mBackgroundColor(ColorA(0, 0, 0, 0)),

	mAlpha(1.0),
	mIsHidden(false),
	mShouldForceRedraw(false),

	mTimeline(Timeline::create()),

	mChildren(),
	mParent(nullptr),

	mSize(0, 0)
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
	mHasInvalidUniforms = true;
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
		invalidateTransforms();
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

	// recalculate transforms if marked as dirty or while animations are happening
	// this way children will also know that their parent transform has changed
	mHasInvalidTransforms = mHasInvalidTransforms || (mParent && mParent->mHasInvalidTransforms);

	if (mHasInvalidTransforms) {
		validateTransforms(false);
	}

	mDrawColor.r = mTint.value().r * parentTint.r;
	mDrawColor.g = mTint.value().g * parentTint.g;
	mDrawColor.b = mTint.value().b * parentTint.b;
	mDrawColor.a = mAlpha.value() * parentTint.a;

	{
		gl::ScopedModelMatrix scopedModelMatrix;
		gl::ScopedViewMatrix scopedViewMatrix;
		//gl::ScopedBlendAlpha scopedBlendAlpha; // bb: no real need for this at this point since we're not changing blend modes a lot

		gl::multModelMatrix(mTransform);
		gl::color(mDrawColor);

		willDraw();
		draw();
		drawChildren(mDrawColor);
		didDraw();
	}

	// clear dirty flag at end so that children know that things have changed
	mHasInvalidTransforms = false;
}

void BaseView::willDraw() {
	// override this method
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

void BaseView::drawChildren(const ColorA& parentTint) {
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
	const vec3 origin = vec3(mTransformOrigin.value(), 0.0f);

	mTransform = glm::translate(vec3(mPosition.value(), 0.0f));
	mTransform *= glm::translate(origin);	// offset by origin
	mTransform *= glm::scale(vec3(mScale.value(), 1.0f));
	mTransform *= glm::toMat4(mRotation.value());
	mTransform *= glm::translate(-origin);	// reset to original position
	mGlobalTransform = mParent ? mParent->mGlobalTransform * mTransform : mTransform;
	if (clearInvalidFlag) {
		mHasInvalidTransforms = false;
	}
}

const vec2 BaseView::convertLocalToGlobal(const vec2& local) const {
	vec4 global = mGlobalTransform * vec4(local, 0, 1);
	return vec2(global.x, global.y);
}

const vec2 BaseView::convertGlobalToLocal(const vec2& global) const {
	vec4 local = glm::inverse(mGlobalTransform) * vec4(global, 0, 1);
	return vec2(local.x, local.y);
}

const vec2 BaseView::getGlobalPosition() const {
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
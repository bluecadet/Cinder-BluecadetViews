//+---------------------------------------------------------------------------
//  Bluecadet Interactive 2016
//	Developers: Benjamin Bojko
//  Contents: 
//  Comments: 
//----------------------------------------------------------------------------

#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Tween.h"
#include "cinder/Timeline.h"

#include "boost/variant.hpp"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class BaseView> BaseViewRef;
typedef std::list<BaseViewRef> BaseViewList;
typedef BaseViewList::size_type BaseViewListIndexType;

class BaseView {
	typedef boost::variant<
		bool, int, float, double,
		ci::ivec2, ci::ivec3, ci::ivec4,
		ci::vec2, ci::vec3, ci::vec4,
		ci::mat2, ci::mat3, ci::mat4, ci::quat,
		std::string
	> UserInfoTypes;
	typedef std::map<std::string, UserInfoTypes> UserInfo;

public:
	struct Event {
		enum class Type { ContentUpdated, Other };
		Type				type = Event::Type::ContentUpdated;
		BaseView*			target = nullptr;
		BaseView*			currentTarget = nullptr;
	};

	BaseView();
	virtual ~BaseView();


	//==================================================
	// Scene graph modification
	// 

	//! Updates this view and all of its children. Call this method on root views that don't have a parent.
	virtual void			updateScene(double deltaTime);

	//! Applies tint color, alpha and matrices and then draws itself and all children. Validates transforms internally.
	virtual void			drawScene(const ci::ColorA& parentTint = ci::ColorA(1.0f, 1.0f, 1.0, 1.0f)) final;

	//! Used for all internal animations
	ci::TimelineRef			getTimeline();

	//! Resets all animatable properties to their defaults and resets all animations. Does not remove children.
	virtual void			reset();

	//! Cancels all running animations on this timeline and this view's animation properties
	virtual void			resetAnimations();

	//! Helper that will dispatch a function after a delay.
	//! The function will be added to the view's timeline and can be canceled with all other animations.
	virtual ci::CueRef		dispatchAfter(std::function<void()> fn, float delay = 0.0f);

	//! Dispatch events as needed. 
	void					dispatchEvent(Event& event);
	//! Override to handle response to dispatched event
	virtual void			handleEvent(const Event& event) {};

	virtual void			addChild(BaseViewRef child);
	virtual void			addChild(BaseViewRef child, size_t index);

	virtual void			removeChild(BaseViewRef child);
	virtual void			removeChild(BaseView* childPtr);
	virtual BaseViewList::iterator removeChild(BaseViewList::iterator childIt);
	virtual void			removeAllChildren();

	//! Gets the child's index or -1 if not found; Mildly expensive with O(N) complexity
	int						getChildIndex(BaseViewRef child);
	virtual void			moveToFront();
	virtual void			moveToBack();
	virtual void			moveChildToIndex(BaseView* childPtr, size_t index);
	virtual void			moveChildToIndex(BaseViewRef child, size_t index);
	virtual void			moveChildToIndex(BaseViewList::iterator childIt, size_t index);



	//==================================================
	// Getters/Setters
	// 

	//! Parent or nullptr
	virtual BaseView*					getParent() const { return mParent; };

	//! Ordered list of all children optimized for fast insertion and removal
	virtual const BaseViewList&			getChildren() const { return mChildren; };
	virtual const size_t				getNumChildren() const { return mChildren.size(); };

	//! Local position relative to parent view
	virtual ci::Anim<ci::vec2>&			getPosition() { return mPosition; };
	virtual void						setPosition(const ci::vec2& position) { mPosition = position; invalidate(); };
	virtual void						setPosition(const ci::vec3& position) { mPosition = ci::vec2(position.x, position.y); invalidate(); };

	//! Shorthand for combining position and size to center the view at `center`
	virtual void						setCenter(const ci::vec2 center) { setPosition(center  - 0.5f * getSize()); };

	//! Shorthand for getting the center based on the current position and size
	virtual ci::vec2					getCenter() { return getPosition().value() + getSize() * 0.5f; };

	//! Local scale relative to parent view
	virtual ci::Anim<ci::vec2>&			getScale() { return mScale; };
	virtual void						setScale(const float& scale) { mScale = ci::vec2(scale, scale);  invalidate(); };
	virtual void						setScale(const ci::vec2& scale) { mScale = scale;  invalidate(); };
	virtual void						setScale(const ci::vec3& scale) { mScale = ci::vec2(scale.x, scale.y);  invalidate(); };

	//! Local rotation relative to parent view. Changing this value invalidates transforms.
	virtual ci::Anim<ci::quat>&			getRotation() { return mRotation; };
	virtual void						setRotation(const float radians) { mRotation = glm::angleAxis(radians, ci::vec3(0, 0, 1)); invalidate(); };
	virtual void						setRotation(const ci::quat& rotation) { mRotation = rotation; invalidate(); };

	//! Acts as the point of origin for all transforms. Essentially allows for rotating and scaling around a specific point. Defaults to (0,0). Changing this value invalidates transforms.
	virtual ci::Anim<ci::vec2>&			getTransformOrigin() { return mTransformOrigin; invalidate(); };
	void								setTransformOrigin(const ci::vec2& value) { mTransformOrigin = value; };

	//! Size of this view. Defaults to 0, 0 and is not affected by children. Does not affect transforms (position, rotation, scale).
	virtual const ci::vec2				getSize() { return mSize; }
	virtual void						setSize(const ci::vec2& size) { mSize = size; invalidate(false, true); };

	//! Width of this view. Defaults to 0 and is not affected by children.
	virtual float						getWidth() { return getSize().x; };
	virtual void						setWidth(const float width) { ci::vec2 s = getSize(); setSize(ci::vec2(width, s.y)); };

	//! Height of this view. Defaults to 0 and is not affected by children.
	virtual float						getHeight() { return getSize().y; };
	virtual void						setHeight(const float height) { ci::vec2 s = getSize(); setSize(ci::vec2(s.x, height)); };

	//! The fill color used when drawing the bounding rect when a size greater than 0, 0 is given.
	virtual ci::Anim<ci::ColorA>&		getBackgroundColor() { return mBackgroundColor; }
	virtual void						setBackgroundColor(const ci::Color color) { mBackgroundColor = ci::ColorA(color, 1.0f); } //! Sets background color with 100% alpha
	virtual void						setBackgroundColor(const ci::ColorA color) { mBackgroundColor = color; }

	//! Applied before each draw together with mAlpha; Defaults to white
	virtual ci::Anim<ci::Color>&		getTint() { return mTint; }
	virtual void						setTint(const ci::Color tint) { mTint = tint; } //! Sets tint while preserving current alpha
	virtual void						setTint(const ci::ColorA tint) { mTint = tint; mAlpha = tint.a; } //! Sets mTint and mAlpha properties

	//! Applied before each draw together with mTint; Gets multiplied with parent alpha; Defaults to 1.0f
	virtual ci::Anim<float>&			getAlpha() { return mAlpha; }
	virtual void						setAlpha(const float alpha) { mAlpha = alpha; }

	//! Disables drawing; Update calls are not affected; Defaults to false
	virtual bool						isHidden() const { return mIsHidden; }
	virtual void						setHidden(const bool isHidden) { mIsHidden = isHidden; }

	//! Forces redrawing even when hidden or mAlpha <= 0; Defaults to false
	virtual bool						shouldForceRedraw() const { return mShouldForceRedraw; }
	virtual void						setShouldForceRedraw(const bool shouldForceRedraw) { mShouldForceRedraw = shouldForceRedraw; }



	//==================================================
	// Coordinate space conversions
	// 

	//! The local transform based on this view's coordinate space. Since this method validates the transforms them before returning it's non-const.
	const ci::mat4&						getTransform()			{ validateTransforms(); return mTransform; }
	
	//! The global transform based on the root view's coordinate space. Since this method validates the transforms them before returning it's non-const.
	const ci::mat4&						getGlobalTransform()	{ validateTransforms(); return mGlobalTransform; }

	//! Global position in the root view's coordinate space.
	const ci::vec2						getGlobalPosition()		{ if (!mParent) return mPosition; return mParent->convertLocalToGlobal(mPosition); };
	
	//! Converts a position from the current view's local space to the root view's global space.
	const ci::vec2						convertLocalToGlobal(const ci::vec2& local) { ci::vec4 global = getGlobalTransform() * ci::vec4(local, 0, 1); return ci::vec2(global.x, global.y); }
	
	//! Converts a position from the root view's global space to the current view's local space.
	const ci::vec2						convertGlobalToLocal(const ci::vec2& global) { ci::vec4 local = glm::inverse(getGlobalTransform()) * ci::vec4(global, 0, 1); return ci::vec2(local); };



	//==================================================
	//! Stores key-based user info. Overrivetes any existing values for this key.
	template <typename T>
	void setUserInfo(const std::string& key, const T& value) { mUserInfo[key] = value; }
	bool hasUserInfo(const std::string& key) {
		auto it = mUserInfo.find(key);
		return it != mUserInfo.end();
	}

	//! Returns user info if it exists for the key. Will return an empty instance of the requested type if the key is not found.
	template <typename T>
	const T& getUserInfo(const std::string& key) const {
		static T defaultValue;
		auto it = mUserInfo.find(key);
		if (it == mUserInfo.end()) return defaultValue;
		return boost::exists<T>(it->second);
	}



protected:

	virtual void update(const double deltaTime);	//! Gets called before draw() and after any parent's update. Override this method to plug into the update loop.

	inline virtual void	willDraw() {};	//! Called by drawScene before draw()
	virtual void		draw();			//! Called by drawScene and allows for drawing content for this node. By default draws a rectangle with the current size and background color (only if x/y /bg-alpha > 0)
	inline virtual void	drawChildren(const ci::ColorA& parentTint); //! Called by drawScene() after draw() and before didDraw(). Implemented at bottom of class.
	inline virtual void	didDraw() {};	//! Called by drawScene after draw()

	inline virtual void didMoveToView(BaseView* parent) {};		//! Called when moved to a parent
	inline virtual void willMoveFromView(BaseView* parent) {};	//! Called when removed from a parent

	const ci::ColorA& getDrawColor() const { return mDrawColor; }	//! The color used for drawing, which is a composite of the alpha and tint colors.

	//! This will recalculate the transformation matrix based on the current position, scale and rotation. Gets called automatically before getTransforms(), getGlobalTransforms() or getGlobalPosition() is called.
	inline void	validateTransforms(const bool force = false);

	//! Marks the transformation matrix (and all of its children's matrices) as invalid. This will cause the matrices to be re-calculated when necessary.
	//! When content is true, marks the content as invalid and will dispatch a content updated event
	inline void invalidate(const bool transforms = true, const bool content = true);

private:

	// Helpers
	inline BaseViewList::iterator getChildIt(BaseViewRef child);
	inline BaseViewList::iterator getChildIt(BaseView* childPtr);

	inline static ci::gl::BatchRef		getDefaultDrawBatch();	//! Default shader batch that draws the background in the default implementation of draw().
	inline static ci::gl::GlslProgRef	getDefaultDrawProg();	//! Default glsl program used by the default batch that draws a rectangular background using background color and size.

	// Properties
	BaseView* mParent;
	BaseViewList mChildren;

	ci::TimelineRef mTimeline;
	ci::Anim<float> mAlpha;
	ci::Anim<ci::Color> mTint;
	ci::Anim<ci::ColorA> mBackgroundColor;
	ci::vec2 mSize;
	bool mIsHidden;
	bool mShouldForceRedraw;

	ci::ColorA mDrawColor;	//! Combines mAlpha and mTint for faster draw

	ci::Anim<ci::vec2> mTransformOrigin;
	ci::Anim<ci::vec2> mPosition;
	ci::Anim<ci::vec2> mScale;
	ci::Anim<ci::quat> mRotation;

	ci::mat4 mTransform;
	ci::mat4 mGlobalTransform;
	bool mHasInvalidTransforms;

	std::map<std::string, UserInfoTypes> mUserInfo;

}; // class BaseView



//==================================================
// Inline implementations to improve speed on frequently used methods
// 

void BaseView::drawChildren(const ci::ColorA& parentTint) {
	for (auto child : mChildren) {
		child->drawScene(parentTint);
	}
}

void BaseView::validateTransforms(const bool force) {
	if (!mHasInvalidTransforms && !force) return;

	const ci::vec3 origin = ci::vec3(mTransformOrigin.value(), 0.0f);

	mTransform = glm::translate(ci::vec3(mPosition.value(), 0.0f))
	 * glm::translate(origin)	// offset by origin
	 * glm::scale(ci::vec3(mScale.value(), 1.0f))
	 * glm::toMat4(mRotation.value())
	 * glm::translate(-origin);	// reset to original position

	mGlobalTransform = mParent ? mParent->getGlobalTransform() * mTransform : mTransform;

	mHasInvalidTransforms = false;
}

inline void BaseView::invalidate(const bool transforms, const bool content) {
	if (transforms) {
		mHasInvalidTransforms = true;
		for (auto &child : mChildren) child->invalidate(true, false);
	}

	if (content) {
		auto event = Event();
		event.type = Event::Type::ContentUpdated;
		dispatchEvent(event);
	}
}

BaseViewList::iterator BaseView::getChildIt(BaseViewRef child) {
	if (!child || child->mParent != this) return mChildren.end();
	return std::find(mChildren.begin(), mChildren.end(), child);
}

BaseViewList::iterator BaseView::getChildIt(BaseView* childPtr) {
	if (!childPtr || childPtr->mParent != this) return mChildren.end();
	return std::find_if(mChildren.begin(), mChildren.end(), [&](BaseViewRef child) {
		return child.get() == childPtr;
	});
}


}
}

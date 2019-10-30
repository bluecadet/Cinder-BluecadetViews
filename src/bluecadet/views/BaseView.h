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
#include "cinder/Signals.h"

#include "boost/variant.hpp"

#include "ViewEvent.h"
#include "AnimOperators.h"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class BaseView> BaseViewRef;
typedef std::list<BaseViewRef> BaseViewList;


//==================================================
// BaseView
// 

class BaseView : public std::enable_shared_from_this<BaseView> {

public:


	//==================================================
	// Typedefs
	// 

	typedef boost::variant<
		bool, int, float, double,
		ci::ivec2, ci::ivec3, ci::ivec4,
		ci::vec2, ci::vec3, ci::vec4,
		ci::mat2, ci::mat3, ci::mat4, ci::quat,
		std::string
	>															UserInfoTypes;
	typedef std::map<std::string, UserInfoTypes>				UserInfo;

	typedef ci::signals::Signal<void(ViewEvent & event)>		EventSignal;
	typedef ci::signals::Connection								EventConnection;
	typedef EventSignal::CallbackFn								EventCallback;


	//==================================================
	// Construct/destruct
	// 

	BaseView();
	virtual ~BaseView();


	//==================================================
	// Global defaults
	//

	//! Defaults to true.
	static bool				sEventPropagationEnabled;

	//! Defaults to true. If set to false, all views that should still dispatch this event
	//! (especially views that are childre nof FboViews) will need this flag to be explicitly set to true.
	static bool				sContentInvalidationEnabled;

	//! Defaults to false. Draws all views with debug color to illustrate the view hierarchy.
	static bool				sDrawDebugInfo;

	//! Defaults to false. When sDrawDebugInfo is set to true, this setting will also draw any invisible views
	static bool				sDrawDebugInfoWhenInvisible;

	enum class BlendMode {
		INHERIT,
		ALPHA,
		PREMULT,
		ADD,
		MULTIPLY,
		DISABLE
	};

	struct FrameInfo {
		double absoluteTime;
		double deltaTime;
		FrameInfo(double absoluteTime = ci::app::getElapsedSeconds(), double deltaTime = 0) : absoluteTime(absoluteTime), deltaTime(deltaTime) {}
	};

	//==================================================
	// Scene graph modification
	// 

	//! Updates this view and all of its children. Call this method on root views that don't have a parent.
	virtual void			updateScene(const FrameInfo & frameInfo = FrameInfo());

	//! Applies tint color, alpha and matrices and then draws itself and all children. Validates transforms internally.
	virtual void			drawScene(const ci::ColorA & parentDrawColor = ci::ColorA(1.0f, 1.0f, 1.0, 1.0f)) final;

	//! Returns a shared pointer to this instance
	inline BaseViewRef		getSharedViewPtr() { return shared_from_this(); }

	//! Resets all animatable properties to their defaults and resets all animations. Does not remove children.
	virtual void			reset();

	//! Cancels all running animations on this timeline and this view's animation properties
	virtual void			cancelAnimations();

	virtual void			addChild(BaseViewRef child);
	virtual void			addChild(BaseViewRef child, size_t index);

	virtual void			removeChild(BaseViewRef child);
	virtual void			removeChild(BaseView* childPtr);
	virtual BaseViewList::iterator removeChild(BaseViewList::iterator childIt);
	virtual void			removeAllChildren();
	virtual void			removeSelf(); // Attempts to remove this view from its parent, if it has one.

	//! Gets the child's index or -1 if not found; Mildly expensive with O(N) complexity
	int						getChildIndex(BaseViewRef child);
	virtual void			moveToFront();
	virtual void			moveToBack();
	virtual void			moveChildToIndex(BaseView* childPtr, size_t index);
	virtual void			moveChildToIndex(BaseViewRef child, size_t index);
	virtual void			moveChildToIndex(BaseViewList::iterator childIt, size_t index);


	//==================================================
	// Events
	// 

	//! Signal that will trigger whenever an event is received or dispatched by this view.
	EventSignal &			getEventSignal(const std::string & type) { return mEventSignalsByType[type]; };
	EventConnection			addEventCallback(const std::string & type, const EventCallback callback) { return mEventSignalsByType[type].connect(callback); };
	void					removeEventCallback(const std::string & type, EventConnection & connection) { connection.disconnect(); };
	//void					removeAllEventCallbacks(const std::string & type) { mEventSignalsByType[type].disconnect_all_slots(); }; // TODO: implement
	//void					removeAllEventCallbacks() { for (auto & signal : mEventSignalsByType) signal.second.disconnect_all_slots(); }; // TODO: implement

	//! Dispatch events to this view's children. Will also trigger the event signal.
	void					dispatchEvent(ViewEvent & event);

	//! Dispatch a ViewEvent of `type` to this view's children. Will also trigger the event signal.
    void					dispatchEvent(const std::string & type) { dispatchEvent(ViewEvent(type, getSharedViewPtr())); };

	//! Helper that will dispatch a function after a delay.
	//! The function will be added to the view's timeline and can be canceled with all other animations.
	virtual ci::CueRef		dispatchAfter(std::function<void()> fn, float delay = 0.0f);

	//! Override to handle dispatched events from children.
	virtual void			handleEvent(ViewEvent & event) {}


	//==================================================
	// Getters/Setters
	// 

	//! Parent or nullptr
	virtual BaseView*					getParent() const { return mParent; }

	//! Ordered list of all children optimized for fast insertion and removal
	virtual const BaseViewList&			getChildren() const { return mChildren; }
	virtual const size_t				getNumChildren() const { return mChildren.size(); }

	//! Local position relative to parent view
	virtual ci::Anim<ci::vec2> &		getPosition() { return mPosition; }
	virtual const ci::vec2 &			getPositionConst() const { return mPosition.value(); }
	virtual void						setPosition(const ci::vec2& position) { mPosition = position; invalidate(); }
	virtual void						setPosition(const ci::vec3& position) { mPosition = ci::vec2(position.x, position.y); invalidate(); }
	virtual void						setPosition(float x, float y) { mPosition = ci::vec2(x, y); invalidate(); }

	//! Shorthand for combining position and size to center the view at `center`
	virtual void						setCenter(const ci::vec2 center) { setPosition(center - 0.5f * getSize()); }

	//! Shorthand for getting the center based on the current position and size
	virtual ci::vec2					getCenter() { return getPosition().value() + 0.5f * getSize(); }

	//! Local scale relative to parent view
	virtual ci::Anim<ci::vec2> &		getScale() { return mScale; }
	virtual const ci::vec2 &			getScaleConst() const { return mScale.value(); }
	virtual void						setScale(float scale) { mScale = ci::vec2(scale, scale);  invalidate(); }
	virtual void						setScale(float scaleX, float scaleY) { mScale = ci::vec2(scaleX, scaleY);  invalidate(); }
	virtual void						setScale(const ci::vec2& scale) { mScale = scale;  invalidate(); }
	virtual void						setScale(const ci::vec3& scale) { mScale = ci::vec2(scale.x, scale.y);  invalidate(); }

	//! Local rotation relative to parent view. Changing this value invalidates transforms.
	virtual ci::Anim<ci::quat> &		getRotation() { return mRotation; }
	virtual const ci::quat &			getRotationConst() { return mRotation.value(); }
	virtual float						getRotationZ() const { return glm::roll(mRotation.value()); }
	virtual void						setRotation(const float radians) { mRotation = glm::angleAxis(radians, ci::vec3(0, 0, 1)); invalidate(); }
	virtual void						setRotation(const ci::quat& rotation) { mRotation = rotation; invalidate(); }

	//! Acts as the point of origin for all transforms.
	//! Essentially allows for rotating and scaling around a specific point.
	//! Changing this value invalidates transforms. Defaults to (0,0).
	virtual ci::Anim<ci::vec2>&			getTransformOrigin() { return mTransformOrigin; }
	void								setTransformOrigin(const ci::vec2& value) { mTransformOrigin = value; invalidate(); }

	//! Sets the transform origin. If true is passed for offset compensation then the position will be updated to compensate for the new origin.
	//! This will have the effect that the view will visually remain locked in position. Passing in false is the same as calling setTransformOrigin(vec2).
	void								setTransformOrigin(const ci::vec2& value, const bool compensateForOffset);

	//! Size of this view. Defaults to 0, 0 and is not affected by children. Does not affect transforms (position, rotation, scale).
	virtual const ci::vec2				getSize() { return mSize; }
	virtual void						setSize(const ci::vec2& size) { mSize = size; invalidate(false, true); }
	inline void							setSize(float width, float height) { setSize(ci::vec2(width, height)); }
	inline void							setSize(float widthAndHeight) { setSize(ci::vec2(widthAndHeight, widthAndHeight)); }

	//! Utility method that sets the size of this view to the most bottom/right extents of all child bounds.
	void								resizeToFit();

	//! Width of this view. Defaults to 0 and is not affected by children.
	virtual float						getWidth() { return getSize().x; }
	virtual void						setWidth(const float width) { ci::vec2 s = getSize(); setSize(ci::vec2(width, s.y)); }

	//! Height of this view. Defaults to 0 and is not affected by children.
	virtual float						getHeight() { return getSize().y; }
	virtual void						setHeight(const float height) { ci::vec2 s = getSize(); setSize(ci::vec2(s.x, height)); }

	//! Combined size and position in parent coordinate space. Set scaled to true to multiply size by scale. Does not include rotation.
	virtual ci::Rectf					getBounds(const bool scaled = false) { return ci::Rectf(getPositionConst(), getPositionConst() + (scaled ? getScaleConst() * getSize() : getSize())); }

	//! The fill color used when drawing the bounding rect when a size greater than 0, 0 is given.
	virtual ci::Anim<ci::ColorA>&		getBackgroundColor() { return mBackgroundColor; }
	virtual void						setBackgroundColor(const ci::Color color) { mBackgroundColor = ci::ColorA(color, 1.0f); invalidate(false, true); } //! Sets background color with 100% alpha
	virtual void						setBackgroundColor(const ci::ColorA color) { mBackgroundColor = color; invalidate(false, true); }

	//! Applied before each draw together with mAlpha; Defaults to white
	virtual ci::Anim<ci::Color>&		getTint() { return mTint; }
	virtual void						setTint(const ci::Color tint) { mTint = tint; invalidate(false, true); } //! Sets tint while preserving current alpha
	virtual void						setTint(const ci::ColorA tint) { mTint = tint; mAlpha = tint.a; invalidate(false, true); } //! Sets mTint and mAlpha properties

	//! Applied before each draw together with mTint; Gets multiplied with parent alpha; Defaults to 1.0f
	virtual ci::Anim<float>&			getAlpha() { return mAlpha; }
	virtual void						setAlpha(const float alpha) { mAlpha = alpha; }

	//! Returns a constant reference of getAlpha(). Allows for const access.
	virtual float						getAlphaConst() const { return mAlpha; }
	
	//! Defaults to inherit (doesn't change the blend mode).
	BlendMode							getBlendMode() const { return mBlendMode; }
	virtual void						setBlendMode(const BlendMode value) { mBlendMode = value; }

	//! Disables drawing; Update calls are not affected; Defaults to false
	virtual bool						isHidden() const { return mIsHidden; }
	virtual void						setHidden(const bool isHidden) { mIsHidden = isHidden; }

	//! Forces redrawing even when hidden or alpha <= 0; Defaults to false
	virtual bool						shouldForceInvisibleDraw() const { return mShouldForceInvisibleDraw; }
	virtual void						setShouldForceInvisibleDraw(const bool value) { mShouldForceInvisibleDraw = value; }

	//! Setting this to false will prevent events from propagating from this view to its parent. Defaults to true.
	bool								shouldPropagateEvents() const { return mShouldPropagateEvents; }
	void								setShouldPropagateEvents(const bool value) { mShouldPropagateEvents = value; }

	//! Setting this to false will prevent content invalidation events from being dispatched from this view. Defaults to true.
	bool								shouldDispatchContentInvalidation() const { return mShouldDispatchContentInvalidation; }
	void								setShouldDispatchContentInvalidation(const bool value) { mShouldDispatchContentInvalidation = value; }


	//==================================================
	// Timeline
	// 

	//! Used for all internal animations. By default, no timeline is created until this method is called.
	inline ci::TimelineRef	getTimeline(bool stepToNow = true);

	//! Optional method to set this view's timeline to a specific timeline. Can be used to share timelines across views.
	inline void				setTimeline(ci::TimelineRef timeline) { mTimeline = timeline; }

	//==================================================
	// Coordinate space conversions
	// 

	//! The local transform based on this view's coordinate space. Since this method validates the transforms them before returning it's non-const.
	const ci::mat4&						getTransform() { validateTransforms(); return mTransform; }

	//! The global transform based on the root view's coordinate space. Since this method validates the transforms them before returning it's non-const.
	const ci::mat4&						getGlobalTransform() { validateTransforms(); return mGlobalTransform; }

	//! A transform that rotates and scales around transform origin. Since this method validates the transforms them before returning it's non-const.
	const ci::mat4&						getRotationScaleTransform() { validateTransforms(); return mRotationScaleTransform; }

	//! Global position in the root view's coordinate space.
	const ci::vec2						getGlobalPosition() { if (!mParent) return mPosition; return mParent->convertLocalToGlobal(mPosition); };

	//! Set the global position in the root view's coordinate space.
	void								setGlobalPosition(const ci::vec2 pos) { if (!mParent) { setPosition(pos); } else { setPosition(mParent->convertGlobalToLocal(pos)); } };

	//! Converts a position from the current view's local space to the root view's global space.
	const ci::vec2						convertLocalToGlobal(const ci::vec2& local) { ci::vec4 global = getGlobalTransform() * ci::vec4(local, 0, 1); return ci::vec2(global.x, global.y); }

	//! Converts a position from the root view's global space to the current view's local space.
	const ci::vec2						convertGlobalToLocal(const ci::vec2& global) { ci::vec4 local = glm::inverse(getGlobalTransform()) * ci::vec4(global, 0, 1); return ci::vec2(local); }
	
	//! This will recalculate the transformation matrix based on the current position, scale and rotation. Gets called automatically before getTransforms(), getGlobalTransforms() or getGlobalPosition() is called.
	inline void	validateTransforms(const bool force = false);

	//==================================================
	// User info
	//

	//! Stores key-based user info. Overwrites any existing values for this key and type.
	template <typename T>
	void setUserInfo(const std::string& key, const T& value) { mUserInfo[key] = value; }

	//! Checks if a user info entry exists for this key.
	bool hasUserInfo(const std::string& key) {
		const auto it = mUserInfo.find(key);
		return it != mUserInfo.end();
	}

	//! Removes a user info entry if it exists for this key.
	void removeUserInfo(const std::string& key) {
		const auto it = mUserInfo.find(key);
		if (it != mUserInfo.end()) {
			mUserInfo.erase(key);
		}
	}

	//! Returns user info if it exists for the key. Will return an empty instance of the requested type if the key is not found.
	template <typename T>
	const T& getUserInfo(const std::string& key) const {
		static T defaultValue;
		auto it = mUserInfo.find(key);
		if (it == mUserInfo.end()) return defaultValue;
		return boost::get<T>(it->second);
	}
		

	//==================================================
	// Debug
	//

	//! Unique ID per view.
	const size_t						getViewId() const { return mViewId; }
	const std::string &					getViewIdStr() const { return mViewIdStr; }

	//! Custom name that can be assigned to view and used for debugging; Defaults to view id string.
	const std::string &					getName() const { return mName; }
	void								setName(const std::string & name) { mName = name; }

	//! Retrieves this class' name via typeinfo()
	const std::string					getClassName(const bool stripNameSpace = true) const;

	//! Determines whether calling drawDebugInfo() should include the classname or not. Defaults to true.
	void  setDebugIncludeClassName(const bool value)	{ mDebugIncludeClassName = value; }
	bool  getDebugIncludeClassName() const				{ return mDebugIncludeClassName; }

protected:

	virtual void		update(const FrameInfo & frameInfo) {};	//! Gets called before draw() and after any parent's update. Override this method to plug into the update loop.

	inline virtual void	willDraw() {}							//! Called by drawScene before draw()
	virtual void		draw();									//! Called by drawScene and allows for drawing content for this node. By default draws a rectangle with the current size and background color (only if x/y /bg-alpha > 0)
	virtual void		drawDebugInfo();						//! Called in sDrawDebugInfo is set to true (Can be controller by SettingsManager params)
	inline virtual void	drawChildren(const ci::ColorA & parentDrawColor); //! Called by drawScene() after draw() and before didDraw(). Implemented at bottom of class.
	inline virtual void	didDraw() {}							//! Called by drawScene after draw()

	inline virtual void didMoveToView(BaseView * parent) {}		//! Called when moved to a parent
	inline virtual void willMoveFromView(BaseView * parent) {}	//! Called when removed from a parent

	const ci::ColorA & getDrawColor() const { return mDrawColor; }	//! The color used for drawing, which is a composite of the alpha and tint colors.
	BlendMode getDrawBlendMode() const { return mDrawBlendMode; };	//! The current applied blend mode. Useful if this view's blend mode is set to inherit. Updated on each render pass.
	
	//! Progresses the timeline
	virtual void			advanceTimeline(ci::TimelineRef timeline, const FrameInfo & frameInfo);

	//! Marks the transformation matrix (and all of its children's matrices) as invalid. This will cause the matrices to be re-calculated when necessary.
	//! When content is true, marks the content as invalid and will dispatch a content updated event
	inline void invalidate(const bool transforms = true, const bool content = true);

	//! True if any properties that visually modifies this view has been changed since the last call of validateContent().
	virtual bool hasInvalidContent() const	{ return mHasInvalidContent; }
	virtual void validateContent()			{ mHasInvalidContent = false; }

private:

	// Helpers
	inline BaseViewList::iterator getChildIt(BaseViewRef child);
	inline BaseViewList::iterator getChildIt(BaseView* childPtr);

	inline static ci::gl::BatchRef		getDefaultDrawBatch();	//! Default shader batch that draws the background in the default implementation of draw().
	inline static ci::gl::GlslProgRef	getDefaultDrawProg();	//! Default glsl program used by the default batch that draws a rectangular background using background color and size.

	static size_t sNumInstances;

	// Properties
	BaseView* mParent;
	BaseViewList mChildren;

	ci::TimelineRef mTimeline;

	ci::Anim<float> mAlpha;
	ci::Anim<ci::Color> mTint;
	ci::Anim<ci::ColorA> mBackgroundColor;
	ci::vec2 mSize;

	bool mIsHidden;
	bool mShouldForceInvisibleDraw;
	BlendMode mBlendMode;
	BlendMode mDrawBlendMode;

	ci::ColorA mDrawColor;	//! Combines mAlpha and mTint for faster draw

	ci::Anim<ci::vec2> mTransformOrigin;
	ci::Anim<ci::vec2> mPosition;
	ci::Anim<ci::vec2> mScale;
	ci::Anim<ci::quat> mRotation;

	ci::mat4 mTransform;				// contains position, transform origin, rotation and scale
	ci::mat4 mGlobalTransform;			// current transform multiplied with parent's transform
	ci::mat4 mRotationScaleTransform;	// contains rotation and scale around transform origin
	bool mHasInvalidTransforms;
	bool mHasInvalidContent;


	// Events
	bool												mShouldDispatchContentInvalidation;
	bool												mShouldPropagateEvents;
	std::map<std::string, EventSignal>					mEventSignalsByType;
	std::map<std::string, std::set<EventConnection>>	mEventConnectionsByType;

	// Misc
	const size_t							mViewId;
	const std::string						mViewIdStr;
	std::string								mName;
	bool									mDebugIncludeClassName;
	std::map<std::string, UserInfoTypes>	mUserInfo;


}; // class BaseView



//==================================================
// Inline implementations to improve speed on frequently used methods
// 

void BaseView::drawChildren(const ci::ColorA& parentDrawColor) {
	for (auto child : mChildren) {
		child->drawScene(parentDrawColor);
	}
}

void BaseView::validateTransforms(const bool force) {
	if (!mHasInvalidTransforms && !force) return;

	const ci::vec3 origin = ci::vec3(mTransformOrigin.value(), 0.0f);

	mRotationScaleTransform = glm::translate(origin)	// offset by origin
		* glm::scale(ci::vec3(mScale.value(), 1.0f))
		* glm::toMat4(mRotation.value())
		* glm::translate(-origin);						// reset to original position

	mTransform = glm::translate(ci::vec3(mPosition.value(), 0.0f))
		* mRotationScaleTransform;

	mGlobalTransform = mParent ? mParent->getGlobalTransform() * mTransform : mTransform;

	mHasInvalidTransforms = false;
}

inline void BaseView::invalidate(const bool transforms, const bool content) {
	if (transforms && !mHasInvalidTransforms) {
		mHasInvalidTransforms = true;
		for (auto child : mChildren) {
			child->invalidate(true, false);
		}
	}

	if (content) {
		mHasInvalidContent = true;
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

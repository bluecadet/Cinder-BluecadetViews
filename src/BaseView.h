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

	BaseView();
	virtual ~BaseView();

	//! Updates this view and all of its children. Call this method on root views that don't have a parent.
	virtual void			updateScene(double deltaTime);

	//! Applies tint color, alpha and matrices and then draws itself and all children. Validates transforms internally.
	virtual void			drawScene(const ci::ColorA& parentColor = ci::ColorA(1.0f, 1.0f, 1.0, 1.0f)) final;

	//! Used for all internal animations
	ci::TimelineRef			getTimeline();

	//! Resets all animatable properties to their defaults and resets all animations. Does not remove children.
	virtual void			reset();

	//! Cancels all running animations on this timeline and this view's animation properties
	virtual void			resetAnimations();

	//! Helper that will dispatch a function after a delay.
	//! The function will be added to the view's timeline and can be canceled with all other animations.
	virtual ci::CueRef		dispatchAfter(std::function<void()> fn, float delay = 0.0f);

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


	// Getters/Setters

	//! Parent or nullptr
	virtual BaseView*					getParent() const { return mParent; }

	//! Ordered list of all children optimized for fast insertion and removal
	virtual const BaseViewList&			getChildren() const { return mChildren; }
	virtual const size_t				getNumChildren() const { return mChildren.size(); }

	//! Local position relative to parent view
	virtual ci::Anim<ci::vec2>&			getPosition() { return mPosition; };
	virtual void						setPosition(const ci::vec2 &position) { mPosition = position; invalidateTransforms(); }
	virtual void						setPosition(const ci::vec3 &position) { mPosition = ci::vec2(position.x, position.y); invalidateTransforms(); }

	//! Local scale relative to parent view
	virtual ci::Anim<ci::vec2>&			getScale() { return mScale; };
	virtual void						setScale(const ci::vec2 &scale) { mScale = scale;  invalidateTransforms(); };
	virtual void						setScale(const ci::vec3 &scale) { mScale = ci::vec2(scale.x, scale.y);  invalidateTransforms(); };

	//! Local rotation relative to parent view
	virtual ci::Anim<ci::quat>&			getRotation() { return mRotation; };
	virtual void						setRotation(const float radians) { mRotation = glm::angleAxis(radians, ci::vec3(0, 0, 1)); invalidateTransforms(); };
	virtual void						setRotation(const ci::quat &rotation) { mRotation = rotation; invalidateTransforms(); };

	virtual void						validateTransforms(const bool clearInvalidFlag = true);
	virtual const ci::mat4&				getTransform() { if (mHasInvalidTransforms) { validateTransforms(); }; return mTransform; }
	virtual const ci::mat4&				getGlobalTransform() { if (mHasInvalidTransforms) { validateTransforms(); }; return mGlobalTransform; }

	//! Applied before each draw together with mAlpha; Defaults to white
	virtual ci::Anim<ci::Color>&		getColor() { return mColor; }
	virtual void						setColor(const ci::Color color) { mColor = color; }
	virtual void						setColor(const ci::ColorA color) { mColor = color; mAlpha = color.a; } //! Sets mColor and mAlpha properties

	//! Applied before each draw together with mColor; Gets multiplied with parent alpha; Defaults to 1.0f
	virtual ci::Anim<float>&			getAlpha() { return mAlpha; }
	virtual void						setAlpha(const float alpha) { mAlpha = alpha; }

	//! Disables drawing; Update calls are not affected; Defaults to false
	virtual bool						isHidden() const { return mIsHidden; }
	virtual void						setHidden(const bool isHidden) { mIsHidden = isHidden; }

	//! Forces redrawing even when hidden or mAlpha <= 0; Defaults to false
	virtual bool						shouldForceRedraw() const { return mShouldForceRedraw; }
	virtual void						setShouldForceRedraw(const bool shouldForceRedraw) { mShouldForceRedraw = shouldForceRedraw; }

	// Conversion helpers
	const ci::vec2 getGlobalPosition() const;	//! Global position regardless of parent view; Gets computed recursively on each update
	const ci::vec2 convertLocalToGlobal(const ci::vec2& local) const;
	const ci::vec2 convertGlobalToLocal(const ci::vec2& global) const;

	// Templated user info functions to store and retrieve arbitrary, key-based user info
	template <typename T>
	void setUserInfo(const std::string& key, const T& value) { mUserInfo[key] = value; }
	bool hasUserInfo(const std::string& key) {
		auto it = mUserInfo.find(key);
		return it != mUserInfo.end();
	}

	template <typename T>
	const T& getUserInfo(const std::string& key) const {
		static T defaultValue;
		auto it = mUserInfo.find(key);
		if (it == mUserInfo.end()) return defaultValue;
		return boost::get<T>(it->second);
	}

protected:

	virtual void invalidateTransforms() { mHasInvalidTransforms = true; };

	virtual void update(const double deltaTime);

	virtual void willDraw();		//! Called by drawScene before draw()
	virtual void draw();			//! Called by drawScene and allows for drawing content for this node
	virtual void drawChildren(const ci::ColorA& parentColor);	//! Called by drawScene() after draw() and before didDraw()
	virtual void didDraw();			//! Called by drawScene after draw()

	virtual void didMoveToView(BaseView* parent);		//! Called when moved to a parent
	virtual void willMoveFromView(BaseView* parent);	//! Called when removed from a parent

	const ci::ColorA& getColorA() const { return mColorA; }

private:

	BaseView* mParent;
	BaseViewList mChildren;

	ci::TimelineRef mTimeline;
	ci::Anim<ci::Color> mColor;
	ci::Anim<float> mAlpha;
	bool mIsHidden;
	bool mShouldForceRedraw;

	ci::Anim<ci::vec2> mPosition;
	ci::Anim<ci::vec2> mScale;
	ci::Anim<ci::quat> mRotation;
	ci::ColorA mColorA; //! For internal use only; Merges mAlpha and mColor for faster draw

	ci::mat4 mTransform;
	ci::mat4 mGlobalTransform;
	bool mHasInvalidTransforms;

	BaseViewList::iterator getChildIt(BaseViewRef child);
	BaseViewList::iterator getChildIt(BaseView* childPtr);

	std::map<std::string, UserInfoTypes> mUserInfo;

};

}
}

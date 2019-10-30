#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Tween.h"

#include "BaseView.h"

namespace bluecadet {
namespace views {

typedef std::shared_ptr<class AnimatedView> AnimatedViewRef;


//! The AnimationView class is a helper class to add a common interface
//! for animating views in and out with guaranteed callbacks and 
//! consistent state management.
//! 
//! The callback is guaranteed to be called, even if the animation
//! itself is canceled or the view is already animated on.
//! 
//! When the animation gets canceled (e.g. when animateOff() gets called)
//! the callback will be triggered with completed=false.
//! 
//! Callbacks are always called asynchronously on the timeline.
class AnimatedView : public BaseView {

public:

	//==================================================
	// Typedefs
	// 
	typedef ci::signals::Signal<void()> VoidSignal;
	typedef ci::signals::Signal<void(bool completed)> CallbackSignal;
	typedef std::function<void(bool completed)> CallbackFn;

	//! AnimatedView options
	struct Options {
		Options clone() const			{ return Options(*this); }

		Options & duration(float v)		{ mDuration = v; return *this; }
		Options & delay(float v)		{ mDelay = v; return *this; }
		Options & easing(ci::EaseFn v)	{ mEasing = v; return *this; }
		
		Options & addDuration(float v)	{ mDuration += v; return *this; }
		Options & addDelay(float v)		{ mDelay += v; return *this; }

		float getDuration() const		{ return mDuration; }
		float getDelay() const			{ return mDelay; }
		ci::EaseFn getEasing() const	{ return mEasing; }

		float getEndTime() const		{ return mDuration + mDelay; }

		/*template <typename T>
		typename ci::Tween<T>::Options & applyTo(typename ci::Tween<T>::Options & tweenOptions) const {
			return tweenOptions.easeFn(mEasing).delay(mDelay);
		};*/

		template <typename T>
		inline typename ci::Tween<T>::Options & apply(ci::TimelineRef timeline,
											   typename ci::Anim<T> * target,
											   T startValue,
											   T endValue) const {
			return timeline->apply(target, startValue, endValue, mDuration, mEasing).delay(mDelay);
		};
		template <typename T>
		inline typename ci::Tween<T>::Options & apply(ci::TimelineRef timeline,
											   typename ci::Anim<T> * target,
											   T endValue) const {
			return timeline->apply(target, endValue, mDuration, mEasing).delay(mDelay);
		};
		template <typename T>
		inline typename ci::Tween<T>::Options & appendTo(ci::TimelineRef timeline,
											   typename ci::Anim<T> * target,
											   T startValue,
											   T endValue) const {
			return timeline->appendTo(target, startValue, endValue, mDuration, mEasing).delay(mDelay);
		};
		template <typename T>
		inline typename ci::Tween<T>::Options & appendTo(ci::TimelineRef timeline,
											   typename ci::Anim<T> * target,
											   T endValue) const {
			return timeline->appendTo(target, endValue, mDuration, mEasing).delay(mDelay);
		};

	private:
		float mDuration = 0.33f;
		float mDelay = 0;
		ci::EaseFn mEasing = ci::EaseInOutQuad();
	};


	//! Cue that is guaranteed to trigger a callback with a completed bool
	class CallbackCue : public ci::Cue {
	public:
		CallbackCue(CallbackFn callback, float time);
		~CallbackCue();

		//! Adds a single callback to the list
		inline void addCallback(CallbackFn callback);

		//! Removes callbacks without triggering them
		inline void removeAllCallbacks() { mConnections.clear(); }

		//! Allows you to manually add callbacks, but connecting here will not allow you to use removeAllCallbacks()
		inline CallbackSignal & getSignalCallback() { return mSignalCallback; }

	protected:
		CallbackSignal mSignalCallback;
		ci::signals::ConnectionList mConnections;
	};
	typedef std::shared_ptr<CallbackCue> CallbackCueRef;


	//==================================================
	// Methods
	// 
	AnimatedView(bool showInitially = true);
	virtual ~AnimatedView();


	//! Animate this view on.
	void animateOn(const Options & options, CallbackFn callback = nullptr, bool isCallbackAsync = true);
	
	//! Animate this view off.
	void animateOff(const Options & options, CallbackFn callback = nullptr, bool isCallbackAsync = true);

	//! Shortcut to animateOn() with default options.
	void animateOn(CallbackFn callback = nullptr, bool isCallbackAsync = true) { animateOn(mDefaultOptions, callback, isCallbackAsync); };

	//! Shortcut to animateOff() with default options.
	void animateOff(CallbackFn callback = nullptr, bool isCallbackAsync = true) { animateOff(mDefaultOptions, callback, isCallbackAsync); };

	//! Same as animating on with a duration of 0.
	void setToAnimatedOn();

	//! Same as animating off with a duration of 0.
	void setToAnimatedOff();


	//! True if the view is currently showing.
	bool isShowing() const { return mIsShowing; }

	//! The target state that this view should have.
	bool shouldShow() const { return mShouldShow; }

	//! True if the view is currently animating to a different state, which means
	//! that shouldShow() and isShowing() are not the same.
	bool isAnimating() const { return mIsShowing != mShouldShow; }

	//! Cancels the current on animation
	void cancelAnimationOn();

	//! Cancels the current off animation
	void cancelAnimationOff();

	//! Cancels all animations (on/off/others).
	void cancelAnimations() override;

	//! Signals triggered at beginning off animations
	VoidSignal & getSignalWillAnimateOn() { return mSignalWillAnimateOn; }
	VoidSignal & getSignalWillAnimateOff() { return mSignalWillAnimateOff; }


	//! Signals triggered at end of animations
	VoidSignal & getSignalDidAnimateOn() { return mSignalDidAnimateOn; }
	VoidSignal & getSignalDidAnimateOff() { return mSignalDidAnimateOff; }

	//! returns default options
	const Options & getDefaultOptions() { return mDefaultOptions; }

protected:
	//! Override this method to add your own animations to the timeline. Keep all animations based on the overall duration.
	virtual void addAnimationOn(ci::TimelineRef timeline, const Options & options);

	//! Override this method to add your own animations to the timeline. Keep all animations based on the overall duration.
	virtual void addAnimationOff(ci::TimelineRef timeline, const Options & options);

	//! Override to prepare for animating on
	virtual void willAnimateOn() {};

	//! Override to respond the completed on-animation
	virtual void didAnimateOn() {};

	//! Override to prepare for animating off
	virtual void willAnimateOff() {};

	//! Override to respond the completed off-animation
	virtual void didAnimateOff() {};

	void didMoveToView(BaseView * view) override;


private:

	CallbackCueRef mCueOn;
	CallbackCueRef mCueOff;

	bool mShouldShow = true;
	bool mIsShowing = true;

	bool mShowInitially = true;
	bool mIsInitialized = false;

	Options mDefaultOptions;

	VoidSignal mSignalWillAnimateOn;
	VoidSignal mSignalWillAnimateOff;

	VoidSignal mSignalDidAnimateOn;
	VoidSignal mSignalDidAnimateOff;

};
}
}

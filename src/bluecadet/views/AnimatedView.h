#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

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
	typedef ci::signals::Signal<void()> VoidSignal;
	typedef ci::signals::Signal<void(bool completed)> CallbackSignal;
	typedef std::function<void(bool completed)> CallbackFn;
	struct Options {

		Options clone() const { return Options(*this); }

		Options & duration(float v) { mDuration = v; return *this; }
		Options & delay(float v) { mDelay = v; return *this; }
		Options & easing(ci::EaseFn v) { mEasing = v; return *this; }

		float getDuration() const { return mDuration; }
		float getDelay() const { return mDelay; }
		ci::EaseFn getEasing() const { return mEasing; }

		float getEndTime() const { return mDuration + mDelay; }

	private:
		float mDuration = 0.33f;
		float mDelay = 0;
		ci::EaseFn mEasing = ci::EaseInOutQuad();
	};


	AnimatedView(bool showInitially = true);
	virtual ~AnimatedView();


	//! Animate this view on.
	void animateOn(const Options & options, CallbackFn callback = nullptr);
	
	//! Animate this view off.
	void animateOff(const Options & options, CallbackFn callback = nullptr);

	//! Shortcut to animateOn() with default options.
	void animateOn(CallbackFn callback = nullptr) { animateOn(mDefaultOptions, callback); };

	//! Shortcut to animateOff() with default options.
	void animateOff(CallbackFn callback = nullptr) { animateOff(mDefaultOptions, callback); };


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


	//! Signals triggered at beginning off animations
	VoidSignal & getSignalWillAnimateOn() { return mSignalWillAnimateOn; }
	VoidSignal & getSignalWillAnimateOff() { return mSignalWillAnimateOff; }


	//! Signals triggered at end of animations
	VoidSignal & getSignalDidAnimateOn() { return mSignalDidAnimateOn; }
	VoidSignal & getSignalDidAnimateOff() { return mSignalDidAnimateOff; }


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
	class CompletionCue : public ci::Cue {
	public:
		CompletionCue(CallbackFn callback, float time) :
			ci::Cue([=] {
				mSignalCallback.emit(true); // success
			}, time)
		{
			mSignalCallback.connect(callback);
		}
		~CompletionCue() {
			if (!isComplete()) {
				mSignalCallback.emit(false); // failure
			}
		}

		CallbackSignal mSignalCallback;
	};
	typedef std::shared_ptr<CompletionCue> CompletionCueRef;


	void cancelAnimationOn();
	void cancelAnimationOff();


	CompletionCueRef mCueOn;
	CompletionCueRef mCueOff;

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

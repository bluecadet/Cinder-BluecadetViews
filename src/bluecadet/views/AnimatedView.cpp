#include "AnimatedView.h"

#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

//==================================================
// Supporting Types
// 

AnimatedView::CallbackCue::CallbackCue(CallbackFn callback, float time) :
	ci::Cue([=] {
	mSignalCallback.emit(true); // success
}, time) {
	addCallback(callback);
}

AnimatedView::CallbackCue::~CallbackCue() {
	if (!isComplete()) {
		mSignalCallback.emit(false); // canceled
	}
}

inline void AnimatedView::CallbackCue::addCallback(CallbackFn callback) {
	mConnections += mSignalCallback.connect(callback);
}

//==================================================
// Main Class
// 

AnimatedView::AnimatedView(bool showInitially) :
	BaseView(),
	mShowInitially(showInitially),
	mShouldShow(showInitially),
	mIsShowing(true)
{
}

AnimatedView::~AnimatedView() {
}

void AnimatedView::animateOn(const Options & options, CallbackFn callback, bool isCallbackAsync) {
	mIsInitialized = true; // override initial state

	CallbackFn wrappedCallback = nullptr;
	
	if (callback) {
		if (isCallbackAsync) {
			wrappedCallback = [=] (bool completed) {
				App::get()->dispatchAsync([=] {
					if (callback) {
						callback(completed);
					}
				});
			};
		} else {
			wrappedCallback = callback;
		}
	}

	if (mIsShowing && mShouldShow) {
		// already showing; dispatch on next frame
		if (wrappedCallback) {
			wrappedCallback(true);
		}
		return;
	}

	if (!mShouldShow || !mCueOn) {
		// start on animation
		mShouldShow = true;

		cancelAnimationOn();
		cancelAnimationOff();

		addAnimationOn(getTimeline(), options);

		mCueOn = make_shared<CallbackCue>([=](bool completed) {
			if (completed) {
				mIsShowing = true;
				didAnimateOn();
				mSignalDidAnimateOn.emit();
			}
			mCueOn = nullptr;

		}, getTimeline()->getCurrentTime() + options.getDuration() + options.getDelay());

		getTimeline()->insert(mCueOn);

		willAnimateOn();
		mSignalWillAnimateOn.emit();
	}

	if (wrappedCallback) {
		if (mCueOn) {
			// add callback (multiple callbacks are possible)
			mCueOn->getSignalCallback().connect(wrappedCallback);
		} else {
			// this can happen if willAnimateOn cancels the animation
			wrappedCallback(false);
		}
	}
}

void AnimatedView::animateOff(const Options & options, CallbackFn callback, bool isCallbackAsync) {
	mIsInitialized = true; // override initial state

	CallbackFn wrappedCallback = nullptr;

	if (callback) {
		if (isCallbackAsync) {
			wrappedCallback = [=] (bool completed) {
				App::get()->dispatchAsync([=] {
					if (callback) {
						callback(completed);
					}
				});
			};
		} else {
			wrappedCallback = callback;
		}
	}

	if (!mIsShowing && !mShouldShow) {
		// already hidden; dispatch on next frame
		if (wrappedCallback) {
			wrappedCallback(true);
		}
		return;
	}

	if (mShouldShow || !mCueOff) {
		// start off animation
		mShouldShow = false;

		cancelAnimationOn();
		cancelAnimationOff();

		addAnimationOff(getTimeline(), options);

		mCueOff = make_shared<CallbackCue>([=](bool completed) {
			if (completed) {
				mIsShowing = false;
				didAnimateOff();
				mSignalDidAnimateOff.emit();
			}
			mCueOff = nullptr;
		}, getTimeline()->getCurrentTime() + options.getDuration() + options.getDelay());

		getTimeline()->insert(mCueOff);

		willAnimateOff();
		mSignalWillAnimateOff.emit();
	}

	if (wrappedCallback) {
		if (mCueOff) {
			// add callback (multiple callbacks are possible)
			mCueOff->getSignalCallback().connect(wrappedCallback);

		} else {
			// this can happen if willAnimateOff cancels the animation
			wrappedCallback(false);
		}
	}
}

void AnimatedView::setToAnimatedOn() {
	animateOn(Options().duration(0).easing(easeNone));
	mShowInitially = true;
}

void AnimatedView::setToAnimatedOff() {
	animateOff(Options().duration(0).easing(easeNone));
	mShowInitially = false;
}

void AnimatedView::addAnimationOn(ci::TimelineRef timeline, const Options & options) {
	timeline->apply(&getAlpha(), 1.0f, options.getDuration(), options.getEasing()).delay(options.getDelay());
}

void AnimatedView::addAnimationOff(ci::TimelineRef timeline, const Options & options) {
	timeline->apply(&getAlpha(), 0.0f, options.getDuration(), options.getEasing()).delay(options.getDelay());
}

void AnimatedView::didMoveToView(BaseView * view) {
	if (!mShowInitially && !mIsInitialized) {
		setToAnimatedOff();
		mIsInitialized = true;
	}
}

void AnimatedView::cancelAnimationOn() {
	if (mCueOn) {
		mCueOn->removeSelf();
	}
	mCueOn = nullptr;
}

void AnimatedView::cancelAnimationOff() {
	if (mCueOff) {
		mCueOff->removeSelf();
	}
	mCueOff = nullptr;
}

void AnimatedView::cancelAnimations() {
	cancelAnimationOn();
	cancelAnimationOff();
	BaseView::cancelAnimations();
}

}
}

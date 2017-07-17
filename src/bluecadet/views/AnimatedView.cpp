#include "AnimatedView.h"

#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace views {

AnimatedView::AnimatedView(bool showInitially) :
	BaseView(),
	mShowInitially(showInitially),
	mShouldShow(showInitially),
	mIsShowing(true)
{
}

AnimatedView::~AnimatedView() {
}

void AnimatedView::animateOn(const Options & options, CallbackFn callback) {
	mIsInitialized = true; // override initial state

	if (mIsShowing && mShouldShow) {
		// already showing; dispatch on next frame
		App::get()->dispatchAsync([=] {
			if (callback) {
				callback(true);
			}
		});
		return;
	}

	if (!mShouldShow || !mCueOn) {
		// start on animation
		mShouldShow = true;

		cancelAnimationOn();
		cancelAnimationOff();

		addAnimationOn(getTimeline(), options);

		mCueOn = CompletionCueRef(new CompletionCue([=](bool completed) {
			if (completed) {
				mIsShowing = true;
				didAnimateOn();
				mSignalDidAnimateOn.emit();
			}
			mCueOn = nullptr;

		}, getTimeline()->getCurrentTime() + options.getDuration() + options.getDelay()));

		getTimeline()->insert(mCueOn);

		willAnimateOn();
		mSignalWillAnimateOn.emit();
	}

	// add callback (multiple callbacks are possible)
	if (callback) {
		mCueOn->mSignalCallback.connect(callback);
	}
}

void AnimatedView::animateOff(const Options & options, CallbackFn callback) {
	mIsInitialized = true; // override initial state

	if (!mIsShowing && !mShouldShow) {
		// already hidden; dispatch on next frame
		App::get()->dispatchAsync([=] {
			if (callback) {
				callback(true);
			}
		});
		return;
	}

	if (mShouldShow || !mCueOff) {
		// start off animation
		mShouldShow = false;

		cancelAnimationOn();
		cancelAnimationOff();

		addAnimationOff(getTimeline(), options);

		mCueOff = CompletionCueRef(new CompletionCue([=](bool completed) {
			if (completed) {
				mIsShowing = false;
				didAnimateOff();
				mSignalDidAnimateOff.emit();
			}
			mCueOff = nullptr;
		}, getTimeline()->getCurrentTime() + options.getDuration() + options.getDelay()));

		getTimeline()->insert(mCueOff);

		willAnimateOff();
		mSignalWillAnimateOff.emit();
	}

	// add callback (multiple callbacks are possible)
	if (callback) {
		mCueOff->mSignalCallback.connect(callback);
	}
}

void AnimatedView::setToAnimatedOn() {
	animateOn(Options().duration(0).easing(easeNone));
	mShowInitially = false;
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

}
}

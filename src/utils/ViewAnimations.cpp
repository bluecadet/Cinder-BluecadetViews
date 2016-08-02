#include "ViewAnimations.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
	namespace views {

		ViewAnimations::ViewAnimations()
		{
		}

		ViewAnimations::~ViewAnimations() {
		}

		ci::Tween<float>::Options ViewAnimations::fadeIn(BaseViewRef view, const float duration, const float delay, ci::EaseFn ease)
		{
			return view->getTimeline()->apply(&view->getAlpha(), 1.0f, duration, ease);
		}

		ci::Tween<float>::Options ViewAnimations::fadeOut(BaseViewRef view, const float duration, const float delay, ci::EaseFn ease)
		{
			return view->getTimeline()->apply(&view->getAlpha(), 0.0f, duration, ease);
		}

		ci::Tween<float>::Options ViewAnimations::fadeTo(BaseViewRef view, const float toVal, const float duration, const float delay, ci::EaseFn ease)
		{
			return view->getTimeline()->apply(&view->getAlpha(), toVal, duration, ease);
		}

		ci::Tween<float>::Options ViewAnimations::fade(BaseViewRef view, const float fromVal, const float toVal, const float duration, const float delay, ci::EaseFn ease)
		{
			return view->getTimeline()->apply(&view->getAlpha(), fromVal, toVal, duration, ease);
		}

		// fadeIn(baseView, toValue = 1, duration = 1.0, delay = 1.0, easing)


	}
}

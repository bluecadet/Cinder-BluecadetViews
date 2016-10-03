#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "BaseView.h"

namespace bluecadet {
	namespace views {

		typedef std::shared_ptr<class ViewAnimations> ViewAnimationsRef;

		class ViewAnimations {

		public:
			ViewAnimations();
			virtual ~ViewAnimations();

			static ci::Tween<float>::Options			fadeIn(BaseViewRef view, const float duration, const float delay = 0.0f, ci::EaseFn ease = ci::easeNone);
			static ci::Tween<float>::Options			fadeOut(BaseViewRef view, const float duration, const float delay = 0.0f, ci::EaseFn ease = ci::easeNone);
			static ci::Tween<float>::Options			fadeTo(BaseViewRef view, const float toVal, const float duration, const float delay = 0.0f, ci::EaseFn ease = ci::easeNone);
			static ci::Tween<float>::Options		fade(BaseViewRef view, const float fromVal, const float toVal, const float duration, const float delay = 0.0f, ci::EaseFn ease = ci::easeNone);

		};

	}
}
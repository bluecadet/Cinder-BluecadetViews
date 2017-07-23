#pragma once

#include "cinder/Tween.h"
#include "cinder/Timeline.h"


//==================================================
// Anim & Value
// 

template<typename T>
T operator + (const ci::Anim<T> & tween, const T & value) {
	return tween.value() + value;
}

template<typename T>
T operator - (const ci::Anim<T> & tween, const T & value) {
	return tween.value() - value;
}

template<typename T>
T operator * (const ci::Anim<T> & tween, const T & value) {
	return tween.value() * value;
}

template<typename T>
T operator / (const ci::Anim<T> & tween, const T & value) {
	return tween.value() / value;
}

template<typename T>
T operator - (const ci::Anim<T> & tween) {
	return -tween.value();
}

//==================================================
// Value & Anim
// 

template<typename T>
T operator + (const T & value, const ci::Anim<T> & tween) {
	return tween.value() + value;
}

template<typename T>
T operator - (const T & value, const ci::Anim<T> & tween) {
	return tween.value() - value;
}

template<typename T>
T operator * (const T & value, const ci::Anim<T> & tween) {
	return tween.value() * value;
}

template<typename T>
T operator / (const T & value, const ci::Anim<T> & tween) {
	return tween.value() / value;
}


//==================================================
// Value Conversion & Anim
// 

template<typename T, typename U>
T operator + (const U & value, const ci::Anim<T> & tween) {
	return value + tween.value();
}

template<typename T, typename U>
T operator - (const U & value, const ci::Anim<T> & tween) {
	return value - tween.value();
}

template<typename T, typename U>
T operator * (const U & value, const ci::Anim<T> & tween) {
	return value * tween.value();
}

template<typename T, typename U>
T operator / (const U & value, const ci::Anim<T> & tween) {
	return value / tween.value();
}


//==================================================
// Anim & Value Conversion
// 

template<typename T, typename U>
T operator + (const ci::Anim<T> & tween, const U & value) {
	return value + tween.value();
}

template<typename T, typename U>
T operator - (const ci::Anim<T> & tween, const U & value) {
	return value - tween.value();
}

template<typename T, typename U>
T operator * (const ci::Anim<T> & tween, const U & value) {
	return value * tween.value();
}

template<typename T, typename U>
T operator / (const ci::Anim<T> & tween, const U & value) {
	return value / tween.value();
}

//==================================================
// Anim & Anim
// 

template<typename T>
T operator + (const ci::Anim<T> & lhs, const ci::Anim<T> & rhs) {
	return lhs.value() + rhs.value();
}

template<typename T>
T operator - (const ci::Anim<T> & lhs, const ci::Anim<T> & rhs) {
	return lhs.value() - rhs.value();
}

template<typename T>
T operator * (const ci::Anim<T> & lhs, const ci::Anim<T> & rhs) {
	return lhs.value() * rhs.value();
}

template<typename T>
T operator / (const ci::Anim<T> & lhs, const ci::Anim<T> & rhs) {
	return lhs.value() / rhs.value();
}

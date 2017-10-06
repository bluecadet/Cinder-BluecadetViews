#pragma once

#include "cinder/Tween.h"

//==================================================
// Anim<T> + T
// 

template<typename T>
inline T operator + (const ci::Anim<T> & tween, const T & value) {
	return tween.value() + value;
}

template<typename T>
inline T operator - (const ci::Anim<T> & tween, const T & value) {
	return tween.value() - value;
}

template<typename T>
inline T operator * (const ci::Anim<T> & tween, const T & value) {
	return tween.value() * value;
}

template<typename T>
inline T operator / (const ci::Anim<T> & tween, const T & value) {
	return tween.value() / value;
}

template<typename T>
inline T operator - (const ci::Anim<T> & tween) {
	return -tween.value();
}

//==================================================
// T + Anim<T>
// 

template<typename T>
inline T operator + (const T & value, const ci::Anim<T> & tween) {
	return value + tween.value();
}

template<typename T>
inline T operator - (const T & value, const ci::Anim<T> & tween) {
	return value - tween.value();
}

template<typename T>
inline T operator * (const T & value, const ci::Anim<T> & tween) {
	return value * tween.value();
}

template<typename T>
inline T operator / (const T & value, const ci::Anim<T> & tween) {
	return value / tween.value();
}


//==================================================
// U + Anim<T>
// 

template<typename T, typename U>
inline T operator + (const U & value, const ci::Anim<T> & tween) {
	return value + tween.value();
}

template<typename T, typename U>
inline T operator - (const U & value, const ci::Anim<T> & tween) {
	return value - tween.value();
}

template<typename T, typename U>
inline T operator * (const U & value, const ci::Anim<T> & tween) {
	return value * tween.value();
}

template<typename T, typename U>
inline T operator / (const U & value, const ci::Anim<T> & tween) {
	return value / tween.value();
}


//==================================================
// Anim<T> + U
// 

template<typename T, typename U>
inline T operator + (const ci::Anim<T> & tween, const U & value) {
	return tween.value() + value;
}

template<typename T, typename U>
inline T operator - (const ci::Anim<T> & tween, const U & value) {
	return tween.value() - value;
}

template<typename T, typename U>
inline T operator * (const ci::Anim<T> & tween, const U & value) {
	return tween.value() * value;
}

template<typename T, typename U>
inline T operator / (const ci::Anim<T> & tween, const U & value) {
	return tween.value() / value;
}


//==================================================
// Anim<T> + Anim<T>
// 

template<typename T>
inline T operator + (const ci::Anim<T> & lhs, const ci::Anim<T> & rhs) {
	return lhs.value() + rhs.value();
}

template<typename T>
inline T operator - (const ci::Anim<T> & lhs, const ci::Anim<T> & rhs) {
	return lhs.value() - rhs.value();
}

template<typename T>
inline T operator * (const ci::Anim<T> & lhs, const ci::Anim<T> & rhs) {
	return lhs.value() * rhs.value();
}

template<typename T>
inline T operator / (const ci::Anim<T> & lhs, const ci::Anim<T> & rhs) {
	return lhs.value() / rhs.value();
}


//==================================================
// ==, !=
// 

template<typename T>
inline bool operator == (const ci::Anim<T> & tween, const T & value) {
	return tween.value() == value;
}

template<typename T>
inline bool operator == (const T & value, const ci::Anim<T> & tween) {
	return value == tween.value();
}

template<typename T>
inline bool operator == (const ci::Anim<T> & lhs, const ci::Anim<T> & rhs) {
	return lhs.value() == rhs.value();
}

template<typename T>
inline bool operator != (const ci::Anim<T> & tween, const T & value) {
	return tween.value() != value;
}

template<typename T>
inline bool operator != (const T & value, const ci::Anim<T> & tween) {
	return value != tween.value();
}

template<typename T>
inline bool operator != (const ci::Anim<T> & lhs, const ci::Anim<T> & rhs) {
	return lhs.value() != rhs.value();
}


//==================================================
// >, <, <=, >=
// 

template<typename T>
inline bool operator > (const ci::Anim<T> & tween, const T & value) {
	return tween.value() > value;
}

template<typename T>
inline bool operator > (const T & value, const ci::Anim<T> & tween) {
	return value > tween.value();
}


template<typename T>
inline bool operator < (const ci::Anim<T> & tween, const T & value) {
	return tween.value() < value;
}

template<typename T>
inline bool operator < (const T & value, const ci::Anim<T> & tween) {
	return value < tween.value();
}


template<typename T>
inline bool operator >= (const ci::Anim<T> & tween, const T & value) {
	return tween.value() >= value;
}

template<typename T>
inline bool operator >= (const T & value, const ci::Anim<T> & tween) {
	return value >= tween.value();
}


template<typename T>
inline bool operator <= (const ci::Anim<T> & tween, const T & value) {
	return tween.value() <= value;
}

template<typename T>
inline bool operator <= (const T & value, const ci::Anim<T> & tween) {
	return value <= tween.value();
}

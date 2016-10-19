////////////////////////////////////////////////////////////////////////////////
//
//  IDEUM
//  Copyright 2011-2013 Ideum
//  All Rights Reserved.
//
//  Gestureworks Core
//
//  File:    GestureWorksCore.cpp
//  Authors:  Ideum
//             
//  NOTICE: Ideum permits you to use, modify, and distribute this file only
//  in accordance with the terms of the license agreement accompanying it.
//
////////////////////////////////////////////////////////////////////////////////

#include "GestureWorksCore.h"

using namespace std;
using namespace ci;
using namespace ci::app;

namespace gwc {

typedef void(*initializeGestureworksType)(int, int);
typedef void(*resizeScreenType)(int, int);
typedef void(*processFrameType)(void);
typedef gwc::PointEventArray* (*consumePointEventsType)(void);
typedef gwc::IntermediateGestureEventArray* (*consumeGestureEventsType)();
typedef bool(*loadGMLType)(char*);
typedef bool(*addGestureType)(char*, char*);
typedef bool(*addGestureSetType)(char*, char*);
typedef bool(*removeGestureType)(char*, char*);
typedef bool(*removeGestureSetType)(char*, char*);
typedef bool(*enableGestureType)(char*, char*);
typedef bool(*disableGestureType)(char*, char*);
typedef bool(*registerWindowForTouchByNameType)(char*);
typedef bool(*registerWindowForTouchType)(HWND hwnd);
typedef void(*registerTouchObjectType)(char*);
typedef bool(*deregisterTouchObjectType)(char*);
typedef bool(*assignTouchPointType)(char*, int);
typedef void(*addEventType)(gwc::touchpoint);

static disableGestureType _disableGesture(0);
static addGestureType _addGesture(0);
static enableGestureType _enableGesture(0);
static loadGMLType _loadGML(0);
static processFrameType _processFrame(0);
static initializeGestureworksType _initializeGestureWorks(0);
static resizeScreenType _resizeScreen(0);
static consumePointEventsType _consumePointEvents(0);
static registerWindowForTouchByNameType _registerWindowForTouchByName(0);
static registerWindowForTouchType _registerWindowForTouch(0);
static registerTouchObjectType _registerTouchObject(0);
static assignTouchPointType _assignTouchPoint(0);
static consumeGestureEventsType _consumeGestureEvents(0);
static deregisterTouchObjectType _deregisterTouchObject(0);
static addGestureSetType _addGestureSet(0);
static removeGestureType _removeGesture(0);
static removeGestureSetType _removeGestureSet(0);
static addEventType _addEvent(0);

static HINSTANCE gwcDLL(0);

int GestureWorks::loadGestureWorks(std::string dll_path) {

	int success = 0;

	gwcDLL = LoadLibraryA(dll_path.c_str());

	if (gwcDLL != NULL) {
		_processFrame = (processFrameType)GetProcAddress(gwcDLL, "processFrame");
		_initializeGestureWorks = (initializeGestureworksType)GetProcAddress(gwcDLL, "initializeGestureWorks");
		_resizeScreen = (resizeScreenType)GetProcAddress(gwcDLL, "resizeScreen");
		_consumePointEvents = (consumePointEventsType)GetProcAddress(gwcDLL, "consumePointEvents");
		_consumeGestureEvents = (consumeGestureEventsType)GetProcAddress(gwcDLL, "consumeGestureEvents");
		_registerWindowForTouchByName = (registerWindowForTouchByNameType)GetProcAddress(gwcDLL, "registerWindowForTouchByName");
		_registerWindowForTouch = (registerWindowForTouchType)GetProcAddress(gwcDLL, "registerWindowForTouch");
		_registerTouchObject = (registerTouchObjectType)GetProcAddress(gwcDLL, "registerTouchObject");
		_assignTouchPoint = (assignTouchPointType)GetProcAddress(gwcDLL, "addTouchPoint");
		_loadGML = (loadGMLType)GetProcAddress(gwcDLL, "loadGML");
		_enableGesture = (enableGestureType)GetProcAddress(gwcDLL, "enableGesture");
		_disableGesture = (disableGestureType)GetProcAddress(gwcDLL, "disableGesture");
		_addGesture = (addGestureType)GetProcAddress(gwcDLL, "addGesture");
		_deregisterTouchObject = (deregisterTouchObjectType)GetProcAddress(gwcDLL, "deregisterTouchObject");
		_addGestureSet = (addGestureSetType)GetProcAddress(gwcDLL, "addGestureSet");
		_removeGesture = (removeGestureType)GetProcAddress(gwcDLL, "removeGesture");
		_removeGestureSet = (removeGestureSetType)GetProcAddress(gwcDLL, "removeGestureSet");
		_addEvent = (addEventType)GetProcAddress(gwcDLL, "addEvent");

		if (
			!_processFrame ||
			!_initializeGestureWorks ||
			!_resizeScreen ||
			!_consumePointEvents ||
			!_consumeGestureEvents ||
			!_registerWindowForTouchByName ||
			!_registerWindowForTouch ||
			!_registerTouchObject ||
			!_assignTouchPoint ||
			!_loadGML ||
			!_enableGesture ||
			!_disableGesture ||
			!_addGesture ||
			!_addGestureSet ||
			!_removeGesture ||
			!_removeGestureSet ||
			!_deregisterTouchObject ||
			!_addEvent

			) {
			success = 2;
		}


	} else {
		success = 1;
	}

	mLoaded = success == 0;

	return success;
}


void GestureWorks::initializeGestureWorks(int screen_width, int screen_height) {
	if (!mLoaded) return;
	_initializeGestureWorks(screen_width, screen_height);
}
void GestureWorks::resizeScreen(int screen_width, int screen_height) {
	if (!mLoaded) return;
	_resizeScreen(screen_width, screen_height);
}
void GestureWorks::processFrame(void) {
	if (!mLoaded) return;
	_processFrame();
}
std::vector<gwc::PointEvent> GestureWorks::consumePointEvents(void) {
	if (!mLoaded) return std::vector<gwc::PointEvent>();
	gwc::PointEventArray* point_events = _consumePointEvents();
	std::vector<gwc::PointEvent> events;
	for (int i = 0; i < point_events->size; i++) {
		events.push_back(point_events->events[i]);
	}
	return events;
}
std::vector<gwc::GestureEvent> GestureWorks::consumeGestureEvents(void) {
	if (!mLoaded) return std::vector<gwc::GestureEvent>();
	gwc::IntermediateGestureEventArray* gesture_events = _consumeGestureEvents();
	std::vector<gwc::GestureEvent> events;
	for (int i = 0; i < gesture_events->size; i++) {
		events.push_back(gwc::GestureEvent(gesture_events->events[i]));
	}
	return events;
}
bool GestureWorks::registerWindowForTouchByName(std::string window_name) {
	if (!mLoaded) return false;
	return _registerWindowForTouchByName((char*)window_name.c_str());
}
bool GestureWorks::registerWindowForTouch(HWND hwnd) {
	if (!mLoaded) return false;
	return _registerWindowForTouch(hwnd);
}
void GestureWorks::registerTouchObject(std::string object_id) {
	if (!mLoaded) return;
	return _registerTouchObject((char*)object_id.c_str());
}
bool GestureWorks::assignTouchPoint(std::string object_id, int point_id) {
	if (!mLoaded) return false;
	return _assignTouchPoint((char*)object_id.c_str(), point_id);
}
bool GestureWorks::loadGML(std::string file_name) {
	if (!mLoaded) return false;
	return _loadGML((char*)file_name.c_str());
}
bool GestureWorks::addGesture(std::string object_id, std::string gesture_id) {
	if (!mLoaded) return false;
	return _addGesture((char*)object_id.c_str(), (char*)gesture_id.c_str());
}
bool GestureWorks::addGestureSet(std::string object_id, std::string set_name) {
	if (!mLoaded) return false;
	return _addGestureSet((char*)object_id.c_str(), (char*)set_name.c_str());
}
bool GestureWorks::removeGesture(std::string object_id, std::string gesture_id) {
	if (!mLoaded) return false;
	return _removeGesture((char*)object_id.c_str(), (char*)gesture_id.c_str());
}
bool GestureWorks::removeGestureSet(std::string object_id, std::string set_name) {
	if (!mLoaded) return false;
	return _removeGestureSet((char*)object_id.c_str(), (char*)set_name.c_str());
}
bool GestureWorks::enableGesture(std::string object_id, std::string gesture_id) {
	if (!mLoaded) return false;
	return _enableGesture((char*)object_id.c_str(), (char*)gesture_id.c_str());
}
bool GestureWorks::disableGesture(std::string object_id, std::string gesture_id) {
	if (!mLoaded) return false;
	return _disableGesture((char*)object_id.c_str(), (char*)gesture_id.c_str());
}
bool GestureWorks::deregisterTouchObject(std::string object_id) {
	if (!mLoaded) return false;
	return _deregisterTouchObject((char*)object_id.c_str());
}
void GestureWorks::addEvent(gwc::touchpoint touch_event) {
	if (!mLoaded) return;
	_addEvent(touch_event);
}

}

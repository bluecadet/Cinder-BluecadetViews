////////////////////////////////////////////////////////////////////////////////
//
//  IDEUM
//  Copyright 2011-2013 Ideum
//  All Rights Reserved.
//
//  Gestureworks Core
//
//  File:    GestureWorksCore.h
//  Authors:  Ideum
//             
//  NOTICE: Ideum permits you to use, modify, and distribute this file only
//  in accordance with the terms of the license agreement accompanying it.
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include <Windows.h>
#include <vector>
#include "GWCUtils.h"

//The following are all of the functions required to use the GestureWorks DLL. 
//Please refer to the documentation for appropriate calling conventions and
//explanation of Event objects.


namespace gwc {

typedef std::shared_ptr<class GestureWorks> GestureWorksRef;

class GestureWorks {

public:
	GestureWorks() {};
	~GestureWorks() {};

	static GestureWorksRef getInstance() {
		static GestureWorksRef instance = nullptr;
		if (!instance) instance = GestureWorksRef(new GestureWorks());
		return instance;
	}

	//! 0 = success, 1 = dll error, 2 = binding error
	int loadGestureWorks(std::string dll_path);
	void initializeGestureWorks(int screen_width, int screen_height);
	void resizeScreen(int screen_width, int screen_height);
	void processFrame(void);
	std::vector<gwc::PointEvent> consumePointEvents(void);
	std::vector<gwc::GestureEvent> consumeGestureEvents(void);
	bool registerWindowForTouchByName(std::string window_name);
	bool registerWindowForTouch(HWND hwnd);
	void registerTouchObject(std::string object_id);
	bool assignTouchPoint(std::string object_id, int point_id);
	bool loadGML(std::string file_name);
	bool addGesture(std::string object_id, std::string gesture_id);
	bool addGestureSet(std::string object_id, std::string set_name);
	bool removeGesture(std::string object_id, std::string gesture_id);
	bool removeGestureSet(std::string object_id, std::string set_name);
	bool enableGesture(std::string object_id, std::string gesture_id);
	bool disableGesture(std::string object_id, std::string gesture_id);
	bool deregisterTouchObject(std::string object_id);
	void addEvent(gwc::touchpoint touch_event);
};
}




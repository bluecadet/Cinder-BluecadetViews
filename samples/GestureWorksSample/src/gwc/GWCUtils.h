////////////////////////////////////////////////////////////////////////////////
//
//  IDEUM
//  Copyright 2011-2013 Ideum
//  All Rights Reserved.
//
//  Gestureworks Core
//
//  File:    GWCUtils.h
//  Authors:  Ideum
//             
//  NOTICE: Ideum permits you to use, modify, and distribute this file only
//  in accordance with the terms of the license agreement accompanying it.
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <map>
#include <vector>
#include <time.h>
#include <windows.h>

namespace gwc
{
	typedef enum {
		TOUCHUPDATE,
		TOUCHADDED,
		TOUCHREMOVED
	} touchStatus;

	class MapStruct {
	public:
		char** names;
		float* values;
		int size;

		MapStruct(int);
		MapStruct();
		~MapStruct();
		MapStruct(const MapStruct&);
		MapStruct& operator= (const MapStruct & other);
	};

	std::map<std::string, float> structToMap(MapStruct);

	class Point {
		float x, y, z, w, h, r;
	public:
		Point( float x, float y, float z, float w, float h, float r);
		Point( float x, float y);
		Point();

		float getX();
		float getY();
		float getZ();
		float getW();
		float getH();
		float getR();
	};

	class PointEvent {

	public:
		int point_id;
		touchStatus status;
		Point position;
		clock_t timestamp;

		PointEvent(int id, touchStatus status, float x, float y, float z, float w, float h, float r);
		PointEvent();
		PointEvent(const PointEvent&);
		PointEvent& operator= (const PointEvent&);
	};

	class PointEventArray {

	public:
		PointEvent* events;
		int size;
		PointEventArray();
		PointEventArray(const PointEventArray&);
		PointEventArray& operator= (const PointEventArray& other);
		~PointEventArray();
	};



	class IntermediateGestureEvent{
	public:
		int ID;
		char* gesture_type;
		char* gesture_id;
		char* target;
		int source;
		int n;
		int hold_n;
		float x;
		float y;
		int timestamp;
		int phase;
		int* locked_points;

		MapStruct values;

		IntermediateGestureEvent();
		IntermediateGestureEvent(const IntermediateGestureEvent&);
		IntermediateGestureEvent& operator= (const IntermediateGestureEvent&);
		~IntermediateGestureEvent();
	};

	class GestureEvent{
	public:
		int ID;
		std::string gesture_type;
		std::string gesture_id;
		std::string target;
		int source;
		int n;
		int hold_n;
		float x;
		float y;
		
		int timestamp;
		int phase;

		std::vector<int> locked_points;
		std::map<std::string,float> values;

		GestureEvent(IntermediateGestureEvent);
		GestureEvent(const GestureEvent&);
		GestureEvent& operator= (const GestureEvent&);
	};


	class IntermediateGestureEventArray{
	public:
		int size;
		IntermediateGestureEvent* events;
		IntermediateGestureEventArray();
		IntermediateGestureEventArray(const IntermediateGestureEventArray&);
		IntermediateGestureEventArray& operator= (const IntermediateGestureEventArray&);
		~IntermediateGestureEventArray();
	};

	class touchpoint{
	public:
		clock_t timestamp;
		touchStatus status;
		long int id;
		float x;
		float y;
		float z;
		float w;
		float h;
		float r;
		void init(long int _id, float _x, float _y, float _z, float _w, float _h);
	};
}
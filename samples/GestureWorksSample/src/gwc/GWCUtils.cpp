////////////////////////////////////////////////////////////////////////////////
//
//  IDEUM
//  Copyright 2011-2013 Ideum
//  All Rights Reserved.
//
//  Gestureworks Core
//
//  File:    GWCUtils.cpp
//  Authors:  Ideum
//             
//  NOTICE: Ideum permits you to use, modify, and distribute this file only
//  in accordance with the terms of the license agreement accompanying it.
//
////////////////////////////////////////////////////////////////////////////////

#include "GWCUtils.h"


gwc::MapStruct::MapStruct(int size) : size(size) {
	if(size > 0) {
		names = (char**) LocalAlloc(LPTR, sizeof(char*) * size);
		values = (float*) LocalAlloc(LPTR, sizeof(float) * size);
		for(int i = 0; i < size; i++)
		{
			names[i] = NULL;
			values[i] = NULL;
		}
	} else { 
		names = NULL;
		values = NULL;
	}
}

gwc::MapStruct::MapStruct() {
	size = 0;
	names = NULL;
	values = NULL;
}

gwc::MapStruct::~MapStruct() {
	if(names != NULL) { 
		for(int i = 0; i < size; i++) {
			LocalFree(names[i]);
		}
		LocalFree(names);
	}
	if(values != NULL) { LocalFree(values); }
}

 gwc::MapStruct& gwc::MapStruct::operator= (const gwc::MapStruct & other)
    {
        if (this != &other)
        {
           this->size = other.size;
			if(this->names != NULL) { LocalFree(names); names = NULL; }
			if(this->values != NULL) { LocalFree(values); values = NULL; }
			if(this->size == 0) { this->names = NULL; this->values = NULL; return *this; }
			this->names = (char**) LocalAlloc(LPTR, sizeof(char*) * this->size);
			this->values = (float*) LocalAlloc(LPTR, sizeof(float) * this->size);
			
            for(int i = 0; i < this->size; i++) {
		
				this->names[i] = (char*) LocalAlloc(LPTR, sizeof(char) * (1+strlen(other.names[i]))); //new char[strlen(other.names[i])];
				std::strncpy(this->names[i],other.names[i], strlen(other.names[i]));
				this->names[i][strlen(other.names[i])] = '\0';
				this->values[i] = other.values[i];
			}
        }
        return *this;
}

gwc::MapStruct::MapStruct(const MapStruct& other) {
	
	size = other.size;
	SIZE_T t = sizeof(char*) * size;
	if(size == 0) { values = NULL; names = NULL; return; }
	names = (char**) LocalAlloc(LPTR, sizeof(char*) * size);
	values = (float*) LocalAlloc(LPTR, sizeof(float) * size);
	
	for(int i = 0; i < size; i++) {
		names[i] = (char*) LocalAlloc(LPTR, sizeof(char) * (1+strlen(other.names[i]))); //new char[strlen(other.names[i])];
		std::strncpy(names[i],other.names[i], strlen(other.names[i]));
		names[i][strlen(other.names[i])] = '\0';
		values[i] = other.values[i];
	}
}

std::map<std::string, float> gwc::structToMap(gwc::MapStruct source){
	std::map<std::string, float> bob = std::map<std::string, float>();
	for(int i = 0; i < source.size; i++) {
		bob.insert(std::pair<std::string,float>(std::string(source.names[i]),source.values[i]));
	}
	return bob;
}

gwc::Point::Point(float x, float y, float z, float w, float h, float r) : x(x), y(y), z(z), w(w), h(h), r(r) {}

gwc::Point::Point(float x, float y) : x(x), y(y) { z, w, h, r = 0; }

gwc::Point::Point() { x, y, z, w, h, r = 0; }

float gwc::Point::getX() {
	return x;
}

float gwc::Point::getY() {
	return y;
}

float gwc::Point::getZ() {
	return z;
}

float gwc::Point::getW() {
	return w;
}

float gwc::Point::getH() {
	return h;
}

float gwc::Point::getR() {
	return r;
}

gwc::PointEvent::PointEvent(int id, touchStatus status, float x, float y, float z, float w, float h, float r) : status(status), point_id(id) {
	position = Point(x, y, z, w, h, r);
}

gwc::PointEvent::PointEvent() {
	point_id = 0; position = Point();
}

gwc::PointEvent::PointEvent(const PointEvent& other) {
	point_id = other.point_id;
	status = other.status;
	position = other.position;
	timestamp = other.timestamp;
}

gwc::PointEvent& gwc::PointEvent::operator= (const gwc::PointEvent& other){
	if(this != &other){
		this->point_id = other.point_id;
		this->status = other.status;
		this->position = other.position;
		this->timestamp = other.timestamp;
	}
	return *this;
}

gwc::PointEventArray::PointEventArray(const gwc::PointEventArray& other){
	size = other.size;
	events = NULL;
	if(other.events != NULL && other.size > 0){
		events = (PointEvent*) LocalAlloc(LPTR, sizeof(PointEvent) * other.size);
		for(int i = 0; i < size; i++){
			events[i] = other.events[i];
		}
	}
}

gwc::PointEventArray& gwc::PointEventArray::operator= (const gwc::PointEventArray& other){
	if(this != &other){
		this->size = other.size;
		if(this->events != NULL){ LocalFree(events); events = NULL; }
		this->events = (PointEvent*) LocalAlloc(LPTR, sizeof(PointEvent) * other.size);
		for(int i = 0; i < this->size; i++){
			this->events[i] = other.events[i];
		}
	}
	return *this;
}

gwc::PointEventArray::PointEventArray(){
	size = 0;
	events = NULL;
}

gwc::PointEventArray::~PointEventArray() {
		if(events != NULL) { LocalFree(events); }
}


gwc::GestureEvent::GestureEvent(const GestureEvent& other) {
	ID = other.ID;
	gesture_type = other.gesture_type;
	gesture_id = other.gesture_id;
	target = other.target;
	source = other.source;
	n = other.n;
	hold_n = other.hold_n;
	x = other.x;
	y = other.y;
	timestamp = other.timestamp;
	phase = other.phase;
	locked_points = other.locked_points;
	values = other.values;
}
gwc::GestureEvent& gwc::GestureEvent::operator= (const GestureEvent& other) {
	if(this != &other) {
		this->ID = other.ID;
		this->gesture_type = other.gesture_type;
		this->gesture_id = other.gesture_id;
		this->target = other.target;
		this->source = other.source;
		this->n = other.n;
		this->hold_n = other.hold_n;
		this->x = other.x;
		this->y = other.y;
		this->timestamp = other.timestamp;
		this->phase = other.phase;
		this->locked_points = other.locked_points;
		this->values = other.values;
	}
	return *this;
}

gwc::IntermediateGestureEvent::IntermediateGestureEvent(){
	ID = 0;
	timestamp = 0;
	gesture_type = NULL;
	target = NULL;
	gesture_id = NULL;
	locked_points = NULL;
	source = -1;
	n = 0;
	hold_n = 0;
	phase = 0;
	x = 0.f;
	y = 0.f;
}

gwc::IntermediateGestureEvent::IntermediateGestureEvent(const IntermediateGestureEvent& e){
	gesture_type = NULL;
	if(e.gesture_type){
		gesture_type = (char*) LocalAlloc(LPTR, sizeof(char) * (1 + strlen(e.gesture_type)));
		std::strncpy(gesture_type, e.gesture_type, strlen(e.gesture_type));
	}
	
	gesture_id = NULL;
	if(e.gesture_id){
		gesture_id = (char*) LocalAlloc(LPTR, sizeof(char) * (1 + strlen(e.gesture_id)));
		std::strncpy(gesture_id, e.gesture_id, strlen(e.gesture_id));
	}
	target = NULL;
	if(e.target){
		target = (char*) LocalAlloc(LPTR, sizeof(char) * (1 + strlen(e.target)));
		std::strncpy(target, e.target, strlen(e.target));
	}
	locked_points = NULL;
	if(e.locked_points){
		locked_points = (int*) LocalAlloc(LPTR, sizeof(int) * e.n);
		for(int i = 0; i < e.n; i++){
			locked_points[i] = e.locked_points[i];
		}
	}

	source = e.source;
	phase = e.phase;
	timestamp = e.timestamp;
	n = e.n;
	ID = e.ID;
	hold_n = e.hold_n;
	x = e.x;
	y = e.y;
	values = MapStruct(e.values);
}

gwc::IntermediateGestureEvent& gwc::IntermediateGestureEvent::operator= (const gwc::IntermediateGestureEvent& other){
	if(this != &other){
		if(this->gesture_type != NULL){ LocalFree(this->gesture_type); gesture_type = NULL; }
		if(this->gesture_id != NULL){ LocalFree(gesture_id); gesture_id = NULL; }
		if(this->target != NULL){ LocalFree(target); target = NULL; }

		if(other.locked_points){
			this->locked_points = (int*) LocalAlloc(LPTR, sizeof(int) * other.n);
			for(int i = 0; i < other.n; i++){
				this->locked_points[i] = other.locked_points[i];
			}
		}
		if(other.target){
			this->target = (char*) LocalAlloc(LPTR, sizeof(char) * (1 + strlen(other.target)));
			std::strncpy(this->target, other.target, strlen(other.target));
		}
		if(other.gesture_type){
			this->gesture_type = (char*) LocalAlloc(LPTR, sizeof(char) * (1 + strlen(other.gesture_type)));
			std::strncpy(this->gesture_type, other.gesture_type, strlen(other.gesture_type));
		}
		if(other.gesture_id){
			this->gesture_id = (char*) LocalAlloc(LPTR, sizeof(char) * (1 + strlen(other.gesture_id)));
			std::strncpy(this->gesture_id, other.gesture_id, strlen(other.gesture_id));
		}
		
		this->source = other.source;
		this->phase = other.phase;
		this->timestamp = other.timestamp;
		this->ID = other.ID;
		this->n = other.n;
		this->hold_n = other.n;
		this->x = other.x;
		this->y = other.y;
		this->values = MapStruct(other.values);
	}
	return *this;
}

gwc::IntermediateGestureEvent::~IntermediateGestureEvent(){
	if(gesture_type != NULL){ LocalFree(gesture_type); }
	if(gesture_id != NULL){ LocalFree(gesture_id); }
	if(target != NULL){ LocalFree(target); }
	if(locked_points != NULL){ LocalFree(locked_points); }
}

gwc::GestureEvent::GestureEvent(IntermediateGestureEvent intermediary) {
	ID = intermediary.ID;
	gesture_type = std::string(intermediary.gesture_type);
	gesture_id = std::string(intermediary.gesture_id);
	target = std::string(intermediary.target);
	source = intermediary.source;
	n = intermediary.n;
	hold_n = intermediary.hold_n;
	x = intermediary.x;
	y = intermediary.y;

	timestamp = intermediary.timestamp;
	phase = intermediary.phase;

	values = structToMap(intermediary.values);

	if(intermediary.locked_points) {
		for(int i = 0; i < intermediary.hold_n; i++) {
			locked_points.push_back(intermediary.locked_points[i]);
		}
	}

}

gwc::IntermediateGestureEventArray::IntermediateGestureEventArray(const gwc::IntermediateGestureEventArray& other){
	size = other.size;
	events = NULL;
	if(other.events != NULL && other.size > 0){
		events = (IntermediateGestureEvent*) LocalAlloc(LPTR, sizeof(IntermediateGestureEvent) * other.size);
		for(int i = 0; i < size; i++){
			events[i] = other.events[i];
		}
	}
}

gwc::IntermediateGestureEventArray& gwc::IntermediateGestureEventArray::operator= (const gwc::IntermediateGestureEventArray& other){
	if(this != &other){
		this->size = other.size;
		if(this->events != NULL){ LocalFree(events); events = NULL; }
		this->events = (IntermediateGestureEvent*) LocalAlloc(LPTR, sizeof(IntermediateGestureEvent) * other.size);
		for(int i = 0; i < this->size; i++){
			this->events[i] = other.events[i];
		}
	}
	return *this;
}

gwc::IntermediateGestureEventArray::IntermediateGestureEventArray(){
	size = 0;
	events = NULL;
}

gwc::IntermediateGestureEventArray::~IntermediateGestureEventArray() {
		if(events != NULL) { LocalFree(events); }
}

void gwc::touchpoint::init(long int _id, float _x, float _y, float _z, float _w, float _h){
	id = _id;
	x = _x;
	y = _y;
	z = _z;
	w = _w;
	h = _h;
	r = sqrt(w*w + h*h);
	timestamp = clock();
}
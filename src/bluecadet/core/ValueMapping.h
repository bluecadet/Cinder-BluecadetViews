#pragma once

#include "cinder/Json.h"
#include "cinder/Log.h"
#include "cinder/app/App.h"
#include "cinder/params/Params.h"

#include <set>

namespace bluecadet {
namespace core {

struct ValueMapping {
	typedef std::function<void(ci::params::InterfaceGlRef params,
							   ValueMapping & mapping)>
		ParamInitFn;

	enum class Type {
		ValueBool,
		ValueUint,
		ValueInt,
		ValueFloat,
		ValueDouble,
		ValueString,
		ValueColor,
		ValueVec2,
		ValueVec3,
		ValueIVec2,
		ValueIVec3
	};

	explicit ValueMapping() {}
	explicit ValueMapping(const std::string & field, void * ptr)
		: field(field), pointer(ptr), type(Type::ValueBool) {}
	explicit ValueMapping(const std::string & field, unsigned int * ptr)
		: field(field), pointer(ptr), type(Type::ValueUint) {}
	explicit ValueMapping(const std::string & field, int * ptr)
		: field(field), pointer(ptr), type(Type::ValueInt) {}
	explicit ValueMapping(const std::string & field, float * ptr)
		: field(field), pointer(ptr), type(Type::ValueFloat) {}
	explicit ValueMapping(const std::string & field, double * ptr)
		: field(field), pointer(ptr), type(Type::ValueDouble) {}
	explicit ValueMapping(const std::string & field, std::string * ptr)
		: field(field), pointer(ptr), type(Type::ValueString) {}
	explicit ValueMapping(const std::string & field, ci::Color * ptr)
		: field(field), pointer(ptr), type(Type::ValueColor) {}
	explicit ValueMapping(const std::string & field, ci::ColorA * ptr)
		: field(field), pointer(ptr), type(Type::ValueColor) {}
	explicit ValueMapping(const std::string & field, ci::vec2 * ptr)
		: field(field), pointer(ptr), type(Type::ValueVec2) {}
	explicit ValueMapping(const std::string & field, ci::vec3 * ptr)
		: field(field), pointer(ptr), type(Type::ValueVec3) {}
	explicit ValueMapping(const std::string & field, ci::ivec2 * ptr)
		: field(field), pointer(ptr), type(Type::ValueIVec2) {}
	explicit ValueMapping(const std::string & field, ci::ivec3 * ptr)
		: field(field), pointer(ptr), type(Type::ValueIVec3) {}

	template <typename T> void setValue(T value) { *(T *)pointer = value; }
	template <typename T> T getValue() const { return *(T *)pointer; };

	ValueMapping & param(const std::string & name,
						 const std::string & group = "",
						 const std::string & key = "",
						 const std::string & options = "");

	ValueMapping & param(ParamInitFn fn);


	ValueMapping & commandArg(const std::string & name);
	ValueMapping & commandArgs(const std::set<std::string> & names);

	ci::JsonTree toJson(const std::string & key) const;
	void readJson(const ci::JsonTree & json);
	void readCommandArg(const std::string & argValue);
	void attachToParams(ci::params::InterfaceGlRef params);

	ci::vec2 strToVec2(const std::string & str);
	ci::ivec2 strToIVec2(const std::string & str);
	ci::vec3 strToVec3(const std::string & str);
	ci::ivec3 strToIVec3(const std::string & str);

	std::string field;
	void * pointer;
	Type type;

	std::string paramName;
	std::string paramGroup;
	std::string paramKey;
	std::string paramOptions;
	ParamInitFn paramFn = nullptr;  // If this is set, param name, group and key
									// will be ignored

	std::set<std::string> commandArgNames;

protected:
	template <typename T> void initParam(ci::params::InterfaceGlRef params);
};

template <typename T>
inline void ValueMapping::initParam(ci::params::InterfaceGlRef params) {
	if (paramFn) {
		paramFn(params, *this);
		return;
	}

	if (paramName.empty()) {
		return;
	}

	ci::params::InterfaceGl::Options<T> options = params->addParam<T>(
		paramName, (T *)pointer);

	if (!paramOptions.empty()) {
		options.optionsStr(paramOptions);
	}
	if (!paramGroup.empty()) {
		options.group(paramGroup);
	}
	if (!paramKey.empty()) {
		options.key(paramKey);
	}
}

}  // namespace core
}  // namespace bluecadet

#include "ValueMapping.h"

#include "bluecadet/text/Text.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace core {

ValueMapping & ValueMapping::param(const std::string & name, const std::string & group, const std::string & key, const std::string & options) {
	paramName = name;
	paramGroup = group;
	paramKey = key;
	paramOptions = options;
	return *this;
}

ValueMapping& ValueMapping::param(ParamInitFn fn) {
	paramFn = fn;
	return *this;
}

ValueMapping& ValueMapping::commandArg(const std::string & name) {
	commandArgNames.insert(name);
	return *this;
}

ValueMapping& ValueMapping::commandArgs(const std::set<std::string> & names) {
	commandArgNames.insert(names.begin(), names.end());
	return *this;
}

JsonTree ValueMapping::toJson(const std::string & key) const {
	try {
		switch (type) {
			case Type::ValueBool: return JsonTree(key, getValue<bool>());
			case Type::ValueUint: return JsonTree(key, getValue<unsigned int>());
			case Type::ValueInt: return JsonTree(key, getValue<int>());
			case Type::ValueFloat: return JsonTree(key, getValue<float>());
			case Type::ValueDouble: return JsonTree(key, getValue<double>());
			case Type::ValueString: return JsonTree(key, getValue<string>());
			case Type::ValueColor: return JsonTree(key, text::colorToHexStr(getValue<ColorA>(), "#"));
			case Type::ValueVec2: {
				const auto value = getValue<vec2>();
				JsonTree json = JsonTree::makeObject(key);
				json.addChild(JsonTree("x", value.x));
				json.addChild(JsonTree("y", value.y));
				return json;
			}
			case Type::ValueIVec2: {
				const auto value = getValue<ivec2>();
				JsonTree json = JsonTree::makeObject(key);
				json.addChild(JsonTree("x", value.x));
				json.addChild(JsonTree("y", value.y));
				return json;
			}
			case Type::ValueVec3: {
				const auto value = getValue<vec3>();
				JsonTree json = JsonTree::makeObject(key);
				json.addChild(JsonTree("x", value.x));
				json.addChild(JsonTree("y", value.y));
				json.addChild(JsonTree("z", value.z));
				return json;
			}
			case Type::ValueIVec3: {
				const auto value = getValue<ivec3>();
				JsonTree json = JsonTree::makeObject(key);
				json.addChild(JsonTree("x", value.x));
				json.addChild(JsonTree("y", value.y));
				json.addChild(JsonTree("z", value.z));
				return json;
			}
			default: CI_LOG_E("Unsupported type for field '" << field << "'"); break;
		}
	} catch (const std::exception & e) {
		CI_LOG_EXCEPTION("Could not create json for key '" << key << "'", e);
	}
	return JsonTree();
}

void ValueMapping::readJson(const ci::JsonTree & json) {
	try {
		switch (type) {
			case Type::ValueBool: setValue(json.getValueForKey<bool>(field)); break;
			case Type::ValueUint: setValue(json.getValueForKey<unsigned int>(field)); break;
			case Type::ValueInt: setValue(json.getValueForKey<int>(field)); break;
			case Type::ValueFloat: setValue(json.getValueForKey<float>(field)); break;
			case Type::ValueDouble: setValue(json.getValueForKey<double>(field)); break;
			case Type::ValueString: setValue(json.getValueForKey<string>(field)); break;
			case Type::ValueColor: setValue(text::hexStrToColor(json.getValueForKey<string>(field))); break;
			case Type::ValueVec2:
				setValue(vec2(json.getValueForKey<float>(field + ".x"), json.getValueForKey<float>(field + ".y")));
				break;
			case Type::ValueVec3:
				setValue(vec3(json.getValueForKey<float>(field + ".x"), json.getValueForKey<float>(field + ".y"), json.getValueForKey<float>(field + ".z")));
				break;
			case Type::ValueIVec2:
				setValue(ivec2(json.getValueForKey<int>(field + ".x"), json.getValueForKey<int>(field + ".y")));
				break;
			case Type::ValueIVec3:
				setValue(ivec3(json.getValueForKey<int>(field + ".x"), json.getValueForKey<int>(field + ".y"), json.getValueForKey<int>(field + ".z")));
				break;
			default: CI_LOG_E("Unsupported type for field '" << field << "'"); break;
		}
	} catch (const std::exception & e) {
		CI_LOG_EXCEPTION("Could not read json field '" << field << "'", e);
	}
}
void ValueMapping::readCommandArg(const std::string& argValue) {
	try {
		switch (type) {
			case Type::ValueBool: setValue(argValue == "true"); break;
			case Type::ValueUint: setValue((unsigned int)stoi(argValue)); break;
			case Type::ValueInt: setValue(stoi(argValue)); break;
			case Type::ValueFloat: setValue(stof(argValue)); break;
			case Type::ValueDouble: setValue(stod(argValue)); break;
			case Type::ValueString: setValue(argValue); break;
			case Type::ValueColor: setValue(text::hexStrToColor(argValue)); break;
			case Type::ValueVec2: setValue(strToVec2(argValue)); break;
			case Type::ValueVec3: setValue(strToVec3(argValue)); break;
			case Type::ValueIVec2: setValue(strToIVec2(argValue)); break;
			case Type::ValueIVec3: setValue(strToIVec3(argValue)); break;
			default: CI_LOG_E("Unsupported type for field '" << field << "'"); break;
		}
	} catch (const std::exception & e) {
		CI_LOG_EXCEPTION("Could not read json field '" << field << "'", e);
	}
}
void ValueMapping::attachToParams(ci::params::InterfaceGlRef params) {
	if (paramName.empty()) {
		return;
	}

	switch (type) {
		case Type::ValueBool: initParam<bool>(params); break;
		case Type::ValueUint: initParam<unsigned int>(params); break;
		case Type::ValueInt: initParam<int>(params); break;
		case Type::ValueFloat: initParam<float>(params); break;
		case Type::ValueDouble: initParam<double>(params); break;
		case Type::ValueString: initParam<string>(params); break;
		case Type::ValueColor: initParam<ColorA>(params); break;
		case Type::ValueVec2: initParam<vec3>(params); break;
		case Type::ValueVec3: initParam<vec3>(params); break;
		//case Type::ValueIVec2: initParam<ivec2>(params); break;
		//case Type::ValueIVec3: initParam<ivec3>(params); break;
		default: CI_LOG_E("Params are not supported for field '" << field << "'"); break;
	}

	if (!paramGroup.empty()) {
		params->setOptions(paramGroup, "opened=false");
	}
}

ci::vec2 ValueMapping::strToVec2(const std::string & str) {
	ci::vec2 result;
	auto segments = text::split(str, ',');
	if (segments.size() == 2) {
		result.x = stof(segments[0]);
		result.y = stof(segments[1]);
	}
	return result;
}

ci::ivec2 ValueMapping::strToIVec2(const std::string& str) {
	ci::ivec2 result;
	auto segments = text::split(str, ',');
	if (segments.size() == 2) {
		result.x = stoi(segments[0]);
		result.y = stoi(segments[1]);
	}
	return result;
}

ci::vec3 ValueMapping::strToVec3(const std::string & str) {
	ci::vec3 result;
	auto segments = text::split(str, ',');
	if (segments.size() == 3) {
		result.x = stof(segments[0]);
		result.y = stof(segments[1]);
		result.y = stof(segments[2]);
	} else if (segments.size() == 2) {
		result.x = stof(segments[0]);
		result.y = stof(segments[1]);
		result.z = 0;
	} else if (segments.size() == 1) {
		result.x = stof(segments[0]);
		result.y = stof(segments[0]);
		result.z = stof(segments[0]);
	}
	return result;
}

ci::ivec3 ValueMapping::strToIVec3(const std::string& str) {
	ci::ivec3 result;
	auto segments = text::split(str, ',');
	if (segments.size() == 3) {
		result.x = stoi(segments[0]);
		result.y = stoi(segments[1]);
		result.y = stoi(segments[2]);
	} else if (segments.size() == 2) {
		result.x = stoi(segments[0]);
		result.y = stoi(segments[1]);
		result.z = 0;
	} else if (segments.size() == 1) {
		result.x = stoi(segments[0]);
		result.y = stoi(segments[0]);
		result.z = stoi(segments[0]);
	}
	return result;
}


}  // namespace core
}  // namespace bluecadet

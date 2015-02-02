#pragma once

#include <string>
#include <cstdint>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

// Json lib forward declare
namespace Json
{
	class Value;
}

/**
* \brief Deserialize functions for native types.
*/
bool deserialize(const Json::Value& node, bool& b);

bool deserialize(const Json::Value& node, int8_t& val);
bool deserialize(const Json::Value& node, int16_t& val);
bool deserialize(const Json::Value& node, int32_t& val);
bool deserialize(const Json::Value& node, int64_t& val);

bool deserialize(const Json::Value& node, uint8_t& val);
bool deserialize(const Json::Value& node, uint16_t& val);
bool deserialize(const Json::Value& node, uint32_t& val);
bool deserialize(const Json::Value& node, uint64_t& val);

bool deserialize(const Json::Value& node, float& f);
bool deserialize(const Json::Value& node, double& d);

/**
* \brief Deserialize functions for standard library types.
*/
bool deserialize(const Json::Value& node, std::string& str);

/**
* \brief Deserialize functions for glm types.
*/
bool deserialize(const Json::Value& node, glm::vec2& val);
bool deserialize(const Json::Value& node, glm::vec3& val);
bool deserialize(const Json::Value& node, glm::vec4& val);
bool deserialize(const Json::Value& node, glm::quat& val);
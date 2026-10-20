#pragma once

#include <string>
#include <concepts>
#include <cstdint>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// Json lib forward declare
namespace Json
{
	class Value;
}

/**
 * \brief Deserialize functions for native types.
 */
bool deserialize(const Json::Value& node, bool& b);

/**
 * \brief Deserializes an integer node.
 *
 * The node has to hold a whole number that fits the target type.
 */
template <typename T>
    requires std::integral<T> && (!std::same_as<T, bool>)
bool deserialize(const Json::Value& node, T& val);

bool deserialize(const Json::Value& node, float& f);
bool deserialize(const Json::Value& node, double& d);

/**
 * \brief Deserializes a named parameter of a json object.
 *
 * The node has to be an object that has a member with the given name.
 */
bool loadParam(const Json::Value& node, const std::string& name, std::string& str);
bool loadParam(const Json::Value& node, const std::string& name, glm::vec3& vec);
bool loadParam(const Json::Value& node, const std::string& name, float& f);
bool loadParam(const Json::Value& node, const std::string& name, bool& b);

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

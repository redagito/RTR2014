#pragma once

#include <string>

#include <glm/glm.hpp>
#include <json/json.h>

bool deserialize(const Json::Value& node, float& f);
bool deserialize(const Json::Value& node, std::string& str);
bool deserialize(const Json::Value& node, glm::vec3& vec);

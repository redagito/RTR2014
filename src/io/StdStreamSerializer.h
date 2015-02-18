#pragma once

#include <ostream>
#include <string>
#include <cstdint>
#include <vector>

#include <glm/glm.hpp>

/**
* \brief Boolean type.
*/
bool serialize(std::ostream& stream, bool value);

/**
* \brief Integer types.
*/
bool serialize(std::ostream& stream, int8_t value);
bool serialize(std::ostream& stream, int16_t value);
bool serialize(std::ostream& stream, int32_t value);
bool serialize(std::ostream& stream, int64_t value);
bool serialize(std::ostream& stream, uint8_t value);
bool serialize(std::ostream& stream, uint16_t value);
bool serialize(std::ostream& stream, uint32_t value);
bool serialize(std::ostream& stream, uint64_t value);

/**
* \brief Floating types.
*/
bool serialize(std::ostream& stream, float value);
bool serialize(std::ostream& stream, double value);

/**
* \brief GLM vector types.
*/
bool serialize(std::ostream& stream, const glm::vec2& value);
bool serialize(std::ostream& stream, const glm::vec3& value);
bool serialize(std::ostream& stream, const glm::vec4& value);

/**
* \brief GLM matrix types.
*/
bool serialize(std::ostream& stream, const glm::mat2& value);
bool serialize(std::ostream& stream, const glm::mat3& value);
bool serialize(std::ostream& stream, const glm::mat4& value);

/**
* \brief String type.
*/
bool serialize(std::ostream& stream, const std::string& value);

/**
* \brief Vector type.
*/
bool serialize(std::ostream& stream, const std::vector<uint8_t>& value);
bool serialize(std::ostream& stream, const std::vector<uint16_t>& value);
bool serialize(std::ostream& stream, const std::vector<uint32_t>& value);
bool serialize(std::ostream& stream, const std::vector<uint64_t>& value);

bool serialize(std::ostream& stream, const std::vector<float>& value);

bool serialize(std::ostream& stream, const std::vector<glm::vec3>& value);
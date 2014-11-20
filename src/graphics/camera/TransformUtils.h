#pragma once

#include <glm/glm.hpp>

class TransformUtils
{
   public:
    static glm::vec3 rotate(const glm::vec3& axis, float amount, const glm::vec3& vector);

    static glm::mat4 createModelMatrix(const glm::vec3& position, const glm::vec3& up,
                                       const glm::vec3& right, const glm::vec3& forward,
                                       float scale);
};
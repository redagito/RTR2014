#include "TransformUtils.h"

#include <glm/gtx/transform.hpp>

glm::vec3 TransformUtils::rotate(const glm::vec3& axis, float amount, const glm::vec3& vector)
{
    glm::mat4 rotation = glm::rotate(glm::mat4(1), amount, axis);

    glm::vec4 vector4 = (rotation * glm::vec4(vector.x, vector.y, vector.z, 1));
    return glm::vec3(vector4.x, vector4.y, vector4.z);
}

glm::mat4 TransformUtils::createModelMatrix(const glm::vec3& position, const glm::vec3& up,
                                            const glm::vec3& right, const glm::vec3& forward,
                                            float scale)
{
    glm::mat4 rotationMatrix =
        glm::mat4(-right.x, -right.y, -right.z, 0.0f, up.x, up.y, up.z, 0.0f, forward.x, forward.y,
                  forward.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    return glm::translate(position) * glm::scale(glm::mat4(1), glm::vec3(scale)) * rotationMatrix;
}

#pragma once

#include <glm/glm.hpp>

class CCamera;
class CMesh;
class CMaterial;
class CDirectionalLight;

class CRenderer
{
   public:
    /**
     * \brief Signals start of draw commands for next frame.
     */
    void startFrame();

    /**
     * \brief Signals end of draw commands for current frame.
     */
    void endrame();

    /**
     * \brief Sets active camera.
     */
    void setCamera(const CCamera& viewer);

    /**
     * \brief Draw call for mesh.
     */
    void draw(const CMesh& mesh, const glm::mat4& translation, const glm::mat4& rotation,
              const glm::mat4& scale, const CMaterial& material);

    /**
     * \brief Draw call for directional lights.
     */
    void draw(const CDirectionalLight& light);
};

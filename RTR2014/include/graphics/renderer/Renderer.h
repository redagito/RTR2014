#pragma once

#include <glm/glm.hpp>

// Required for inheritance
#include "graphics/IRenderer.h"

class IGraphicsResourceManager;
class Material;
class Mesh;
class ShaderProgram;

/**
* \brief Abstract renderer base class.
*/
class Renderer : public IRenderer
{
   public:
    Renderer();

    /**
    * \brief Cleans up resource callbacks.
    */
    virtual ~Renderer();

    /**
    * \brief Draws whole scene.
    *
	* Implemented in specialized rendering classes.
    */
    virtual void draw(const IScene& scene, const ICamera& camera, const IWindow& window, const IGraphicsResourceManager& manager) = 0;
    /**
    * \brief Performs GL draw call based on mesh data.
    *
    * Sets vertex array object active and performs either indexed or non-indexed draw call.
    * Shader must be set by caller.
    */
    static void draw(const Mesh* mesh);

    /**
    * \brief Sends transformation matrices and material textures to the shader.
    *
    * Textures the material does not provide fall back to the manager's defaults.
    * Shader must be set active by caller.
    */
    static void bindMaterial(ShaderProgram* shader, const Material* material,
                             const glm::mat4& translation, const glm::mat4& rotation,
                             const glm::mat4& scale, const IGraphicsResourceManager& manager);

};

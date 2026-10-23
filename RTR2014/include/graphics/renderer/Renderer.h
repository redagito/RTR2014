#pragma once

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <string>

// Required for inheritance
#include "graphics/IRenderer.h"

class IGraphicsResourceManager;
class Material;
class Mesh;
class ShaderProgram;
class Texture;

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
    virtual void draw(const IScene& scene, const ICamera& camera, const IWindow& window,
                      const IGraphicsResourceManager& manager, float deltaTime) = 0;
    /**
     * \brief Performs GL draw call based on mesh data.
     *
     * Sets vertex array object active and performs either indexed or non-indexed draw call.
     * Shader must be set by caller.
     */
    static void draw(const Mesh* mesh);

    /**
     * \brief Binds a texture to a texture unit and sets the sampler of the shader.
     *
     * The texture of the material is used, the fallback when it has none. The
     * shader has to have the uniform, a mismatch is an error.
     */
    static void bindTexture(ShaderProgram* shader, const std::string& uniformName, GLint unit,
                            const Texture* texture, const Texture* fallback);
};

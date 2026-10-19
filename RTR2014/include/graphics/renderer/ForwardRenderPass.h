#pragma once

#include <glm/glm.hpp>

#include "graphics/renderer/Transformer.h"
#include "resource/ResourceConfig.h"

class IGraphicsResourceManager;
class IResourceManager;
class IScene;
class ISceneQuery;
class ICamera;
class IWindow;
class Material;
class Mesh;
class ShaderProgram;

/**
* \brief Renders the scene in a single forward pass.
*
* Each object is drawn with the default forward shader, or with the custom
* shader of its material when it defines one.
*/
class ForwardRenderPass
{
   public:
    ForwardRenderPass();
    ~ForwardRenderPass();

    /**
    * \brief Initializes the forward render pass resources.
    */
    bool init(IResourceManager* manager);

    /**
    * \brief Draws the visible scene objects.
    */
    void render(const IScene& scene, const ICamera& camera, const IWindow& window,
                const IGraphicsResourceManager& manager);

   private:
    /**
    * \brief Draws a mesh with the shader of its material.
    */
    void draw(Mesh* mesh, const glm::mat4& translation, const glm::mat4& rotation,
              const glm::mat4& scale, Material* material,
              const IGraphicsResourceManager& manager);

    Transformer m_transformer;

    glm::mat4 m_currentView = glm::mat4(1.f);       /**< Stores the current view matrix. */
    glm::mat4 m_currentProjection = glm::mat4(1.f); /**< Stores the current projection matrix. */
    ResourceId m_forwardShader = invalidResource;    /**< Forward shader resource id. */
    ShaderProgram* m_currentShader = nullptr;        /**< Currently active shader object. */
};
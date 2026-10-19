#pragma once


// Required by inheritance
#include <glm/glm.hpp>

#include "Renderer.h"

#include "resource/ResourceConfig.h"

class Material;
class ShaderProgram;
class IResourceManager;

/**
* \brief Forward renderer.
*/
class ForwardRenderer : public Renderer
{
   public:
    /**
    * \brief Default constructor.
    */
    ForwardRenderer();

    /**
    * \brief Internal resource cleanup.
    */
    ~ForwardRenderer();

    /**
    * \brief Draws whole scene.
    * Implements the draw call as specified by the renderer interface.
    * This function implements the highlevel drawing logic for the scene,
    * selects rendering targets and manages render passes.
    */
	void draw(const IScene& scene, const ICamera& camera, const IWindow& window, const IGraphicsResourceManager& manager);

    /**
    * \brief Performs resource aqucisition and initialization.
    */
    bool init(IResourceManager* manager);

    /**
    * \brief Creates and initializes forward renderer.
    */
    static ForwardRenderer* create(IResourceManager* manager);

   protected:
    /**
    * \brief Draws mesh to current rendering target.
    * Selects the rendering method (indexed, direct, etc.) based on the available mesh data
    * and performs actual draw call.
    */
    void draw(Mesh* mesh, const glm::mat4& translation, const glm::mat4& rotation,
              const glm::mat4& scale, Material* material, const IGraphicsResourceManager& manager);

   private:
    bool initDefaultShaders(IResourceManager* manager);

    glm::mat4 m_currentView = glm::mat4(1.f);       /**< Stores the current view matrix. */
    glm::mat4 m_currentProjection = glm::mat4(1.f); /**< Stores the current projection matrix. */
    ResourceId m_forwardShader = invalidResource;                     /**< Forward shader resource id. */
    ShaderProgram* m_currentShader = nullptr;      /**< Currently active shader object. */
};

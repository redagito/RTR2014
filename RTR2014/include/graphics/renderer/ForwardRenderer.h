#pragma once


// Required for inheritance
#include "Renderer.h"

#include "graphics/renderer/ForwardRenderPass.h"

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
	void draw(const IScene& scene, const ICamera& camera, const IWindow& window, const IGraphicsResourceManager& manager) override;

    /**
     * \brief Performs resource aqucisition and initialization.
     */
    bool init(IResourceManager* manager);

    /**
     * \brief Creates and initializes forward renderer.
     */
    static ForwardRenderer* create(IResourceManager* manager);

   private:
    ForwardRenderPass m_forwardRenderPass;
};
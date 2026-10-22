#pragma once

#include <memory>

// Required for inheritance
#include "Renderer.h"
#include "graphics/renderer/DisplayPass.h"
#include "graphics/renderer/ForwardRenderPass.h"
#include "graphics/renderer/FrameBuffer.h"
#include "graphics/renderer/RenderBuffer.h"

class IResourceManager;
class Texture;

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
    void draw(const IScene& scene, const ICamera& camera, const IWindow& window,
              const IGraphicsResourceManager& manager) override;

    /**
     * \brief Performs resource aqucisition and initialization.
     */
    bool init(IResourceManager* manager, const std::string& skyCubemapFile = std::string());

    /**
     * \brief Creates and initializes forward renderer.
     */
    static std::unique_ptr<ForwardRenderer> create(
        IResourceManager* manager, const std::string& skyCubemapFile = std::string());

   private:
    ForwardRenderPass m_forwardRenderPass;
    DisplayPass m_displayPass;

    // The scene is rendered into a buffer that holds the linear radiance, it
    // is mapped to the display range by the display pass.
    FrameBuffer m_sceneBuffer;
    std::shared_ptr<Texture> m_sceneTexture;
    std::shared_ptr<RenderBuffer> m_depthBuffer;
};
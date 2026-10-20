#pragma once

#include <memory>

#include "graphics/renderer/FrameBuffer.h"
#include "resource/ResourceConfig.h"

class IGraphicsResourceManager;
class IResourceManager;
class IScene;
class ICamera;
class IWindow;
class ShaderProgram;
class Texture;

/**
 * \brief Renders a shadow map from a directional light.
 *
 * Produces a depth map of the scene as seen from the light, used for shadow
 * lookup when lighting the directional light volume.
 */
class ShadowMapPass
{
   public:
    ShadowMapPass();
    ~ShadowMapPass();

    /**
     * \brief Initializes the shadow map pass resources.
     */
    bool init(IResourceManager* manager);

    /**
     * \brief Renders the shadow map from the given camera position.
     */
    void render(const IScene& scene, const ICamera& camera, const IWindow& window,
                const IGraphicsResourceManager& manager);

    /**
     * \brief Returns the shadow map depth texture.
     */
    const std::shared_ptr<Texture>& getShadowMap() const;

   private:
    ResourceId m_shadowMapPassShaderId = -1;
    ShaderProgram* m_shadowMapPassShader = nullptr;
    FrameBuffer m_shadowMapBuffer;
    std::shared_ptr<Texture> m_shadowDepthTexture = nullptr;
};
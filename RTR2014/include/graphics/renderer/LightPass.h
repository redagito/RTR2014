#pragma once

#include <memory>

#include "graphics/renderer/DirectionalLightPass.h"
#include "graphics/renderer/FrameBuffer.h"
#include "graphics/renderer/PointLightPass.h"

class IGraphicsResourceManager;
class IResourceManager;
class IScene;
class ISceneQuery;
class ICamera;
class IWindow;
class Texture;

/**
* \brief Accumulates the scene lighting into the light buffer.
*
* The buffer is initialised with the ambient light, then every point and
* directional light is added on top with additive blending.
*/
class LightPass
{
   public:
    LightPass();
    ~LightPass();

    /**
    * \brief Initializes the light pass resources.
    */
    bool init(IResourceManager* manager);

    /**
    * \brief Renders all scene lights into the light buffer.
    */
    void render(const IScene& scene, const ICamera& camera, const IWindow& window,
                const IGraphicsResourceManager& manager, ISceneQuery& query,
                const std::shared_ptr<Texture>& depthTexture,
                const std::shared_ptr<Texture>& normalSpecularTexture);

    /**
    * \brief Returns the light buffer texture.
    */
    const std::shared_ptr<Texture>& getLightTexture() const;

   private:
    FrameBuffer m_lightPassFrameBuffer;
    std::shared_ptr<Texture> m_lightPassTexture = nullptr;

    PointLightPass m_pointLightPass;
    DirectionalLightPass m_directionalLightPass;
};
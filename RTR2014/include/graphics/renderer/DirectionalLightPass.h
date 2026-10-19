#pragma once

#include <memory>

#include "graphics/renderer/FrameBuffer.h"
#include "graphics/renderer/ShadowMapPass.h"
#include "graphics/renderer/Transformer.h"
#include "resource/ResourceConfig.h"

class IGraphicsResourceManager;
class IResourceManager;
class IScene;
class ISceneQuery;
class ICamera;
class IWindow;
class Mesh;
class ShaderProgram;
class Texture;

/**
* \brief Renders directional lights into the light buffer.
*
* Each shadow casting directional light gets its own shadow map, which is
* sampled per pixel when the light is drawn.
*/
class DirectionalLightPass
{
   public:
    DirectionalLightPass();
    ~DirectionalLightPass();

    /**
    * \brief Initializes the directional light pass resources.
    */
    bool init(IResourceManager* manager);

    /**
    * \brief Renders the directional lights into the light buffer.
    */
    void render(const IScene& scene, const ICamera& camera, const IWindow& window,
                const IGraphicsResourceManager& manager, ISceneQuery& query,
                FrameBuffer& lightBuffer, const std::shared_ptr<Texture>& depthTexture,
                const std::shared_ptr<Texture>& normalSpecularTexture);

   private:
    ResourceId m_directionalLightPassShaderId = -1;
    ResourceId m_directionalLightScreenQuadId = -1;

    ShadowMapPass m_shadowMapPass;

    Transformer m_transformer;
};
#pragma once

#include <memory>

#include "graphics/renderer/FrameBuffer.h"
#include "graphics/renderer/ShadowCubePass.h"
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
* \brief Renders point light volumes into the light buffer.
*
* Each shadow casting point light gets its own shadow cube map, which is
* sampled per pixel when the light volume is drawn.
*/
class PointLightPass
{
   public:
    PointLightPass();
    ~PointLightPass();

    /**
    * \brief Initializes the point light pass resources.
    */
    bool init(IResourceManager* manager);

    /**
    * \brief Renders the point light volumes into the light buffer.
    */
    void render(const IScene& scene, const ICamera& camera, const IWindow& window,
                const IGraphicsResourceManager& manager, ISceneQuery& query,
                FrameBuffer& lightBuffer, const std::shared_ptr<Texture>& depthTexture,
                const std::shared_ptr<Texture>& normalSpecularTexture);

   private:
    ResourceId m_pointLightPassShaderId = -1;
    ResourceId m_pointLightSphereId = -1;

    ShadowCubePass m_shadowCubePass;

    Transformer m_transformer;
};
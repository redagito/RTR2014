#pragma once

#include <memory>

#include "resource/ResourceConfig.h"

class IGraphicsResourceManager;
class IResourceManager;
class IScene;
class ICamera;
class IWindow;
class Texture;

/**
* \brief Applies distance fog to a scene texture.
*/
class FogPass
{
   public:
    FogPass();
    ~FogPass();

    /**
    * \brief Initializes the fog pass resources.
    */
    bool init(IResourceManager* manager);

    /**
    * \brief Renders the scene texture with fog applied.
    */
    void render(const ICamera& camera, const IWindow& window,
                const IGraphicsResourceManager& manager, const std::shared_ptr<Texture>& texture,
                const std::shared_ptr<Texture>& depthTexture);

   private:
    ResourceId m_fogPassShaderId = -1;
    ResourceId m_screenQuadId = -1;
};
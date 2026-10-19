#pragma once

#include <memory>

#include "resource/ResourceConfig.h"

class IGraphicsResourceManager;
class IResourceManager;
class IWindow;
class Texture;

/**
* \brief Applies FXAA anti-aliasing to a scene texture.
*/
class FxaaPass
{
   public:
    FxaaPass();
    ~FxaaPass();

    /**
    * \brief Initializes the FXAA pass resources.
    */
    bool init(IResourceManager* manager);

    /**
    * \brief Renders the scene texture with FXAA applied.
    */
    void render(const IWindow& window, const IGraphicsResourceManager& manager,
                const std::shared_ptr<Texture>& texture);

   private:
    ResourceId m_fxaaPassShaderId = -1;
    ResourceId m_screenQuadId = -1;
};
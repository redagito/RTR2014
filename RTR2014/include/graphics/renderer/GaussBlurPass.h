#pragma once

#include <memory>

#include "resource/ResourceConfig.h"

class IGraphicsResourceManager;
class IResourceManager;
class IWindow;
class Texture;

/**
 * \brief Separable gaussian blur of a scene texture.
 *
 * The vertical and horizontal passes are applied in sequence to produce a
 * symmetric blur.
 */
class GaussBlurPass
{
   public:
    GaussBlurPass();
    ~GaussBlurPass();

    /**
     * \brief Initializes the gaussian blur pass resources.
     */
    bool init(IResourceManager* manager);

    /**
     * \brief Blurs the scene texture vertically.
     */
    void renderVertical(const IWindow& window, const IGraphicsResourceManager& manager,
                        const std::shared_ptr<Texture>& texture);

    /**
     * \brief Blurs the scene texture horizontally.
     */
    void renderHorizontal(const IWindow& window, const IGraphicsResourceManager& manager,
                          const std::shared_ptr<Texture>& texture);

   private:
    ResourceId m_gaussBlurVerticalShaderId = -1;
    ResourceId m_gaussBlurHorizontalShaderId = -1;
    ResourceId m_screenQuadId = -1;
};
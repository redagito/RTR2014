#pragma once

#include <memory>

#include "resource/ResourceConfig.h"

class IGraphicsResourceManager;
class IResourceManager;
class IWindow;
class Texture;

/**
 * \brief Draws a texture to the screen.
 *
 * The passthrough render copies the texture to the currently bound framebuffer,
 * the display render targets the default framebuffer.
 */
class DisplayPass
{
   public:
    DisplayPass();
    ~DisplayPass();

    /**
     * \brief Initializes the display pass resources.
     */
    bool init(IResourceManager* manager);

    /**
     * \brief Copies the texture to the currently bound framebuffer.
     *
     * The texture holds the linear radiance of the scene, it is mapped to the
     * display range, unless the texture holds data that is not a radiance.
     * The measurement of the brightness of the frame sets the exposure.
     */
    void render(const IWindow& window, const IGraphicsResourceManager& manager,
                const std::shared_ptr<Texture>& texture, bool toneMap = true,
                const std::shared_ptr<Texture>& exposureTexture = nullptr);

    /**
     * \brief Draws the texture to the screen.
     */
    void renderToScreen(const IWindow& window, const IGraphicsResourceManager& manager,
                        const std::shared_ptr<Texture>& texture, bool toneMap = true,
                        const std::shared_ptr<Texture>& exposureTexture = nullptr);

   private:
    ResourceId m_displayPassShaderId = -1;
    ResourceId m_screenQuadId = -1;
};
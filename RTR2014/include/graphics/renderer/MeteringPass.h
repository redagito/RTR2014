#pragma once

#include <memory>

#include "graphics/renderer/FrameBuffer.h"
#include "resource/ResourceConfig.h"

class IGraphicsResourceManager;
class IResourceManager;
class IWindow;
class Texture;

/**
 * \brief Measures the brightness of the frame.
 *
 * The pass writes the logarithm of the luminance of the frame into a small
 * texture, the average of it is the geometric mean of the frame. The top level
 * of its mipmap chain holds the measurement, it is used to set the exposure of
 * the display.
 */
class MeteringPass
{
   public:
    MeteringPass();
    ~MeteringPass();

    /**
     * \brief Initializes the metering pass resources.
     */
    bool init(IResourceManager* manager);

    /**
     * \brief Measures the brightness of the texture.
     */
    void render(const IWindow& window, const IGraphicsResourceManager& manager,
                const std::shared_ptr<Texture>& texture);

    /**
     * \brief Returns the texture that holds the measurement.
     */
    const std::shared_ptr<Texture>& getMeasurementTexture() const;

   private:
    ResourceId m_meteringPassShaderId = -1;
    ResourceId m_screenQuadId = -1;

    FrameBuffer m_measurementBuffer;
    std::shared_ptr<Texture> m_measurementTexture = nullptr;
};

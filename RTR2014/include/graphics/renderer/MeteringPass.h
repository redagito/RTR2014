#pragma once

#include <array>
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
 * texture, the average of it is the geometric mean of the frame. The average is
 * blended into a history texture so that the exposure adapts over time instead
 * of changing with every frame.
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
                const std::shared_ptr<Texture>& texture, float deltaTime);

    /**
     * \brief Returns the texture that holds the adapted measurement.
     */
    const std::shared_ptr<Texture>& getMeasurementTexture() const;

   private:
    ResourceId m_meteringPassShaderId = -1;
    ResourceId m_exposureAdaptationShaderId = -1;
    ResourceId m_screenQuadId = -1;

    FrameBuffer m_measurementBuffer;
    FrameBuffer m_adaptationBuffer;
    std::shared_ptr<Texture> m_measurementTexture = nullptr;
    std::array<std::shared_ptr<Texture>, 2> m_adaptedExposureTextures;
    int m_currentExposureIndex = 0;
    bool m_hasAdaptedExposure = false;
};

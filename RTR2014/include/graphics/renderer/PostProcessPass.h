#pragma once

#include <memory>

#include "graphics/renderer/DepthOfFieldPass.h"
#include "graphics/renderer/DisplayPass.h"
#include "graphics/renderer/FogPass.h"
#include "graphics/renderer/FxaaPass.h"
#include "graphics/renderer/GaussBlurPass.h"
#include "graphics/renderer/FrameBuffer.h"

class IGraphicsResourceManager;
class IResourceManager;
class IScene;
class ICamera;
class IWindow;
class Texture;

/**
* \brief Runs the post processing chain on the lit scene.
*
* Applies FXAA, fog and depth of field in sequence, using ping-pong buffers
* to pass the result between the steps.
*/
class PostProcessPass
{
   public:
    PostProcessPass();
    ~PostProcessPass();

    /**
    * \brief Initializes the post processing resources.
    */
    bool init(IResourceManager* manager);

    /**
    * \brief Runs the post processing chain and returns the result.
    */
    void render(const ICamera& camera, const IWindow& window,
                const IGraphicsResourceManager& manager, const std::shared_ptr<Texture>& texture,
                const std::shared_ptr<Texture>& depthTexture,
                const glm::mat4& inverseViewProjection);

    /**
    * \brief Returns the texture holding the post processed scene.
    */
    const std::shared_ptr<Texture>& getOutputTexture() const;

   private:
    FxaaPass m_fxaaPass;
    FogPass m_fogPass;
    GaussBlurPass m_gaussBlurPass;
    DepthOfFieldPass m_depthOfFieldPass;
    DisplayPass m_displayPass;

    FrameBuffer m_postProcessPassFrameBuffer0;
    FrameBuffer m_postProcessPassFrameBuffer1;
    FrameBuffer m_postProcessPassFrameBuffer2;

    std::shared_ptr<Texture> m_postProcessPassTexture0 = nullptr;
    std::shared_ptr<Texture> m_postProcessPassTexture1 = nullptr;
    std::shared_ptr<Texture> m_postProcessPassTexture2 = nullptr;
    std::shared_ptr<Texture> m_postProcessPassOutputTexture = nullptr;
};
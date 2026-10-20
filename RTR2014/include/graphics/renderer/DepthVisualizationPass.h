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
 * \brief Visualizes the depth buffer for debugging.
 */
class DepthVisualizationPass
{
   public:
    DepthVisualizationPass();
    ~DepthVisualizationPass();

    /**
     * \brief Initializes the depth visualization pass resources.
     */
    bool init(IResourceManager* manager);

    /**
     * \brief Renders the depth buffer to the screen.
     * zNear/zFar are the camera clipping planes used to linearize the depth values.
     */
    void render(const IWindow& window, const IGraphicsResourceManager& manager,
                const std::shared_ptr<Texture>& depthTexture, float zNear, float zFar);

   private:
    ResourceId m_visualizeDepthPassShaderId = -1;
    ResourceId m_screenQuadId = -1;
};
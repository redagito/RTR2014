#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "resource/ResourceConfig.h"

class IGraphicsResourceManager;
class IResourceManager;
class IScene;
class ICamera;
class IWindow;
class Texture;

/**
* \brief Blends a scene texture with a blurred copy based on depth.
*/
class DepthOfFieldPass
{
   public:
    DepthOfFieldPass();
    ~DepthOfFieldPass();

    /**
    * \brief Initializes the depth-of-field pass resources.
    */
    bool init(IResourceManager* manager);

    /**
    * \brief Combines the sharp scene texture with the blurred one.
    */
    void render(const ICamera& camera, const IWindow& window,
                const IGraphicsResourceManager& manager,
                const std::shared_ptr<Texture>& sceneTexture,
                const std::shared_ptr<Texture>& blurTexture,
                const std::shared_ptr<Texture>& depthTexture,
                const glm::mat4& inverseViewProjection);

   private:
    ResourceId m_depthOfFieldPassShaderId = -1;
    ResourceId m_screenQuadId = -1;
};
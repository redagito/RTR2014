#pragma once

#include <memory>
#include <string>

#include "graphics/Cubemap.h"
#include "resource/ResourceConfig.h"

class IGraphicsResourceManager;
class IResourceManager;
class ICamera;
class IWindow;
class Mesh;

/**
 * \brief Draws the environment of the scene as a background.
 *
 * The sky is a cubemap that is sampled with the direction of the view. It is
 * drawn before the scene, without a depth, so that it fills the background and
 * is never lit by the lights of the scene.
 */
class SkyPass
{
   public:
    SkyPass();
    ~SkyPass();

    /**
     * \brief Loads the cubemap of the sky and the shader that draws it.
     *
     * The file holds a horizontal cross atlas of the six faces of the cubemap.
     */
    bool init(IResourceManager* manager, const std::string& cubemapFile);

    /**
     * \brief Draws the sky into the frame buffer that is bound.
     */
    void render(const IWindow& window, const IGraphicsResourceManager& manager,
                const ICamera& camera);

    /**
     * \brief Returns whether the sky is usable.
     */
    bool isValid() const;

   private:
    std::shared_ptr<Cubemap> m_cubemap = nullptr;
    ResourceId m_skyShaderId = invalidResource;
    ResourceId m_screenQuadId = invalidResource;
    bool m_valid = false;
};

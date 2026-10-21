#pragma once

#include <memory>

#include "graphics/renderer/FrameBuffer.h"
#include "resource/ResourceConfig.h"

class IGraphicsResourceManager;
class IResourceManager;
class IScene;
class ICamera;
class IWindow;
class Mesh;
class ShaderProgram;
class Texture;

/**
 * \brief Combines the lit scene with the g-buffer to produce the final scene.
 */
class IlluminationPass
{
   public:
    IlluminationPass();
    ~IlluminationPass();

    /**
     * \brief Initializes the illumination pass resources.
     */
    bool init(IResourceManager* manager);

    /**
     * \brief Renders the illuminated scene.
     */
    void render(const IWindow& window, const IGraphicsResourceManager& manager,
                const std::shared_ptr<Texture>& lightTexture,
                const std::shared_ptr<Texture>& diffuseGlowTexture,
                const std::shared_ptr<Texture>& depthTexture);

    /**
     * \brief Returns the texture holding the illuminated scene.
     */
    const std::shared_ptr<Texture>& getOutputTexture() const;

   private:
    ResourceId m_illuminationPassShaderId = -1;
    ResourceId m_illuminationPassScreenQuadId = -1;
    FrameBuffer m_illumationPassFrameBuffer;
    std::shared_ptr<Texture> m_illuminationPassTexture = nullptr;
};
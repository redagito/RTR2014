#include "graphics/renderer/LightPass.h"

#include "graphics/ICamera.h"
#include "graphics/IGraphicsResourceManager.h"
#include "graphics/IScene.h"
#include "graphics/ISceneQuery.h"
#include "graphics/IWindow.h"
#include "graphics/Texture.h"
#include "graphics/renderer/RenderBuffer.h"
#include "graphics/renderer/core/RendererCoreConfig.h"
#include "resource/IResourceManager.h"
#include "util/FatalError.h"
#include "util/Profiler.h"

LightPass::LightPass() { return; }

LightPass::~LightPass() { return; }

void LightPass::render(const IScene& scene, const ICamera& camera, const IWindow& window,
                       const IGraphicsResourceManager& manager, ISceneQuery& query,
                       const std::shared_ptr<Texture>& depthTexture,
                       const std::shared_ptr<Texture>& normalSpecularTexture)
{
    // Prepare light pass frame buffer
    glViewport(0, 0, window.getWidth(), window.getHeight());
    // Resize
    m_lightPassFrameBuffer.resize(window.getWidth(), window.getHeight());
    // Enable light buffer
    m_lightPassFrameBuffer.setActive(GL_FRAMEBUFFER);

    // Retrieve ambient light
    glm::vec3 ambientColor;
    float ambientIntensity;
    scene.getAmbientLight(ambientColor, ambientIntensity);
    // Initialize light buffer with ambient light
    glClearColor(ambientColor.x * ambientIntensity, ambientColor.y * ambientIntensity,
                 ambientColor.z * ambientIntensity, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // No depth testing for light volumes
    glDisable(GL_DEPTH_TEST);
    // Additive blending for light accumulation
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    // Draw point light volumes
    {
        ScopedTimer timer(profiler(), "pointLightPass");
        m_pointLightPass.render(scene, camera, window, manager, query, m_lightPassFrameBuffer,
                                depthTexture, normalSpecularTexture);
    }

    // Draw directional lights
    {
        ScopedTimer timer(profiler(), "directionalLightPass");
        m_directionalLightPass.render(scene, camera, window, manager, query, m_lightPassFrameBuffer,
                                      depthTexture, normalSpecularTexture);
    }

    // Reset state and cleanup
    glDisable(GL_BLEND);
    m_lightPassFrameBuffer.setInactive(GL_FRAMEBUFFER);
}

bool LightPass::init(IResourceManager* manager)
{
    if (!m_pointLightPass.init(manager))
    {
        fatalError("Failed to initialize point light pass.");
    }

    if (!m_directionalLightPass.init(manager))
    {
        fatalError("Failed to initialize directional light pass.");
    }

    // Attach texture for light data
    m_lightPassTexture = std::make_shared<Texture>();
    if (!m_lightPassTexture->init(800, 600, GL_RGBA16F))
    {
        fatalError("Failed to create color texture for light pass frame buffer.");
    }
    m_lightPassFrameBuffer.attach(m_lightPassTexture, GL_COLOR_ATTACHMENT0);

    // Attach depth buffer
    // Depth gets discarded, depth values from geometry pass are used
    std::shared_ptr<RenderBuffer> depthBuffer = std::make_shared<RenderBuffer>();
    if (!depthBuffer->init(800, 600, GL_DEPTH_COMPONENT))
    {
        fatalError("Failed to create depth buffer for light pass frame buffer.");
    }
    m_lightPassFrameBuffer.attach(depthBuffer, GL_DEPTH_ATTACHMENT);
    return true;
}

const std::shared_ptr<Texture>& LightPass::getLightTexture() const { return m_lightPassTexture; }
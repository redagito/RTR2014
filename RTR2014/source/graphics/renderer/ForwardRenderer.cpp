#include "graphics/renderer/ForwardRenderer.h"

#include <cassert>
#include <string>

#include "graphics/ICamera.h"
#include "graphics/IGraphicsResourceManager.h"
#include "graphics/IScene.h"
#include "graphics/ISceneQuery.h"
#include "graphics/IWindow.h"
#include "graphics/Texture.h"
#include "graphics/renderer/core/RendererCoreConfig.h"
#include "resource/IResourceManager.h"
#include "util/FatalError.h"
#include "util/Profiler.h"

ForwardRenderer::ForwardRenderer() { return; }

ForwardRenderer::~ForwardRenderer()
{
    // TODO Cleanup
}

const char* ForwardRenderer::getName() const { return "Forward"; }

bool ForwardRenderer::init(IResourceManager* manager, const std::string& skyCubemapFile)
{
    // Set clear color
    glClearColor(0.6f, 0.6f, 0.6f, 1.0f);

    // Depth
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Backface culling disabled for debugging
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Winding order, standard is counter-clockwise
    glFrontFace(GL_CCW);

    // Load and init default shaders
    if (!m_forwardRenderPass.init(manager, skyCubemapFile))
    {
        fatalError("Failed to initialize the forward render pass.");
    }

    // The scene is rendered into a buffer that holds the linear radiance.
    m_sceneTexture = std::make_shared<Texture>();
    if (!m_sceneTexture->init(800, 600, GL_RGBA16F))
    {
        fatalError("Failed to initialize the scene texture.");
    }
    m_depthBuffer = std::make_shared<RenderBuffer>();
    if (!m_depthBuffer->init(800, 600, GL_DEPTH_COMPONENT24))
    {
        fatalError("Failed to initialize the depth buffer.");
    }
    m_sceneBuffer.attach(m_sceneTexture, GL_COLOR_ATTACHMENT0);
    m_sceneBuffer.attach(m_depthBuffer, GL_DEPTH_ATTACHMENT);
    m_sceneBuffer.setInactive(GL_FRAMEBUFFER);

    // The display pass maps the radiance to the display range.
    if (!m_displayPass.init(manager))
    {
        fatalError("Failed to initialize the display pass.");
    }

    // The metering pass measures the brightness of the frame, it sets the
    // exposure of the display.
    if (!m_meteringPass.init(manager))
    {
        fatalError("Failed to initialize the metering pass.");
    }

    return true;
}

void ForwardRenderer::draw(const IScene& scene, const ICamera& camera, const IWindow& window,
                           const IGraphicsResourceManager& manager, float deltaTime)
{
    // Draw init
    window.setActive();

    // Initializiation
    // The scene is rendered into the buffer that holds the linear radiance.
    m_sceneBuffer.resize(window.getWidth(), window.getHeight());
    m_sceneBuffer.setActive(GL_FRAMEBUFFER);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // Reset viewport
    glViewport(0, 0, window.getWidth(), window.getHeight());

    {
        ScopedTimer timer(profiler(), "forwardRenderPass");
        m_forwardRenderPass.render(scene, camera, window, manager);
    }

    m_sceneBuffer.setInactive(GL_FRAMEBUFFER);

    // Measure the brightness of the frame, it sets the exposure of the display
    {
        ScopedTimer timer(profiler(), "meteringPass");
        m_meteringPass.render(window, manager, m_sceneTexture, deltaTime);
    }

    // The radiance is mapped to the display range.
    {
        ScopedTimer timer(profiler(), "displayPass");
        m_displayPass.renderToScreen(window, manager, m_sceneTexture, true,
                                     m_meteringPass.getMeasurementTexture());
    }
}

std::unique_ptr<ForwardRenderer> ForwardRenderer::create(IResourceManager* manager,
                                                         const std::string& skyCubemapFile)
{
    auto renderer = std::make_unique<ForwardRenderer>();
    if (!renderer->init(manager, skyCubemapFile))
    {
        fatalError("Failed to create forward renderer, initialization failed.");
    }
    return renderer;
}

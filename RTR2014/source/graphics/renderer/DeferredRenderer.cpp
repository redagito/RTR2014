#include <spdlog/spdlog.h>
#include "graphics/renderer/DeferredRenderer.h"

#include <string>

#include "graphics/renderer/core/RendererCoreConfig.h"

#include "util/Profiler.h"

#include "resource/IResourceManager.h"

#include "graphics/IScene.h"
#include "graphics/ISceneQuery.h"
#include "graphics/ICamera.h"
#include "graphics/IWindow.h"
#include "graphics/IGraphicsResourceManager.h"


DeferredRenderer::DeferredRenderer() { return; }

DeferredRenderer::~DeferredRenderer() { return; }

bool DeferredRenderer::init(IResourceManager* manager)
{
    spdlog::info("Initializing deferred renderer.");


    // Init geometry pass resources
    if (!m_geometryPass.init(manager))
    {
        spdlog::error("Failed to intialize geometry pass.");
        return false;
    }

    // Init light pass resources
    if (!m_lightPass.init(manager))
    {
        spdlog::error("Failed to initialize light pass.");
        return false;
    }

    // Init illumination pass
    if (!m_illuminationPass.init(manager))
    {
        spdlog::error("Failed to initialize illumination pass.");
        return false;
    }
    // Post process
    if (!m_postProcessPass.init(manager))
    {
        spdlog::error("Failed to initialize post processing pass.");
        return false;
    }

    // Display pass
    if (!m_displayPass.init(manager))
    {
        spdlog::error("Failed to initialize display pass.");
        return false;
    }

    if (!m_depthVisualizationPass.init(manager))
    {
        spdlog::error("Failed to initialize depth visualization pass.");
        return false;
    }
    return true;
}

void DeferredRenderer::draw(const IScene& scene, const ICamera& camera, const IWindow& window,
                             const IGraphicsResourceManager& manager)
{
    ScopedTimer frameTimer(profiler(), "frame");

    // Draw init
    window.setActive();

    // Query visible scene objects and lights
    std::unique_ptr<ISceneQuery> query(
        std::move(scene.createQuery(camera.getProjection() * camera.getView())));

    // Geometry pass fills gbuffer
    {
        ScopedTimer timer(profiler(), "geometryPass");
        m_geometryPass.render(scene, *query, camera, window, manager);
    }

    // Light pass fills lbuffer
    {
        ScopedTimer timer(profiler(), "lightPass");
        m_lightPass.render(scene, camera, window, manager, *query,
                           m_geometryPass.getDepthTexture(),
                           m_geometryPass.getNormalSpecularTexture());
    }

    // Illumination pass renders lit scene from lbuffer and gbuffer
    {
        ScopedTimer timer(profiler(), "illuminationPass");
        m_illuminationPass.render(window, manager, m_lightPass.getLightTexture(),
                                  m_geometryPass.getDiffuseGlowTexture(),
                                  m_geometryPass.getDepthTexture());
    }

    // Post processing pass
    {
        ScopedTimer timer(profiler(), "postProcessPass");
        m_postProcessPass.render(camera, window, manager, m_illuminationPass.getOutputTexture(),
                                  m_geometryPass.getDepthTexture());
    }

    FrameBuffer::setDefaultActive();
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    // Select rendering mode
    if (camera.getFeatureInfo().renderMode == RenderMode::Color)
    {
        m_displayPass.renderToScreen(window, manager, m_geometryPass.getDiffuseGlowTexture());
    }
    else if (camera.getFeatureInfo().renderMode == RenderMode::Depth)
    {
        m_depthVisualizationPass.render(window, manager, m_geometryPass.getDepthTexture());
    }
    else if (camera.getFeatureInfo().renderMode == RenderMode::Lights)
    {
        m_displayPass.renderToScreen(window, manager, m_lightPass.getLightTexture());
    }
    else if (camera.getFeatureInfo().renderMode == RenderMode::Normals)
    {
        m_displayPass.renderToScreen(window, manager, m_geometryPass.getNormalSpecularTexture());
    }
    else  // Final mode
    {
        // Final display pass
        m_displayPass.renderToScreen(window, manager, m_postProcessPass.getOutputTexture());
    }

    // Post draw error check
}

DeferredRenderer* DeferredRenderer::create(IResourceManager* manager)
{
    DeferredRenderer* renderer = new DeferredRenderer;
    if (!renderer->init(manager))
    {
        delete renderer;
        renderer = nullptr;
        spdlog::error("Failed to create deferred renderer, initialization failed.");
    }
    return renderer;
}


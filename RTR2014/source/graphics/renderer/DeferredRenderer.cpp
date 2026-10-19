#include <spdlog/spdlog.h>
#include "graphics/renderer/DeferredRenderer.h"

#include <string>

#include "graphics/renderer/core/RendererCoreConfig.h"

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

    // Shadow mapping
    // TODO Should be done in directional light pass
    if (!m_shadowMapPass.init(manager))
    {
        spdlog::error("Failed to intialize shadow map pass.");
        return false;
    }

    if (!m_shadowCubePass.init(manager))
    {
        spdlog::error("Failed to intialize shadow map pass.");
        return false;
    }

    // Init light pass resources
    if (!initPointLightPass(manager))
    {
        spdlog::error("Failed to initialize point light pass.");
        return false;
    }

    // Init directional light pass
    if (!initDirectionalLightPass(manager))
    {
        spdlog::error("Failed to initialize directional light pass.");
        return false;
    }

    // Init illumination pass
    if (!initIlluminationPass(manager))
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
    // Draw init
    window.setActive();

    // Camera state shared by the passes
    m_transformer.setViewMatrix(camera.getView());
    m_transformer.setProjectionMatrix(camera.getProjection());

    // Query visible scene objects and lights
    std::unique_ptr<ISceneQuery> query(std::move(scene.createQuery(camera)));

    // Geometry pass fills gbuffer
    m_geometryPass.render(scene, *query, camera, window, manager);

    // Light pass fills lbuffer
    lightPass(scene, camera, window, manager, *query);

    // Illumination pass renders lit scene from lbuffer and gbuffer
    illuminationPass(scene, camera, window, manager, *query);

    // Post processing pass
    m_postProcessPass.render(camera, window, manager, m_illuminationPassTexture,
                              m_geometryPass.getDepthTexture(),
                              m_transformer.getInverseViewProjectionMatrix());

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
        m_displayPass.renderToScreen(window, manager, m_lightPassTexture);
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


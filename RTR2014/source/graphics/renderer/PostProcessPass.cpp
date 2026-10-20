#include <spdlog/spdlog.h>
#include "graphics/renderer/PostProcessPass.h"

#include <string>

#include "graphics/renderer/core/RendererCoreConfig.h"

#include "util/Profiler.h"

#include "resource/IResourceManager.h"

#include "graphics/ICamera.h"
#include "graphics/IWindow.h"
#include "graphics/IGraphicsResourceManager.h"
#include "graphics/Texture.h"

PostProcessPass::PostProcessPass()
{
    return;
}

PostProcessPass::~PostProcessPass()
{
    return;
}

void PostProcessPass::render(const ICamera& camera, const IWindow& window,
                             const IGraphicsResourceManager& manager,
                             const std::shared_ptr<Texture>& texture,
                             const std::shared_ptr<Texture>& depthTexture)
{
    // Set view and projection matrices
    m_transformer.setViewMatrix(camera.getView());
    m_transformer.setProjectionMatrix(camera.getProjection());

    // Reset viewport
    glViewport(0, 0, window.getWidth(), window.getHeight());
    // Resize frame buffer
    m_postProcessPassFrameBuffer0.resize(window.getWidth(), window.getHeight());
    m_postProcessPassFrameBuffer1.resize(window.getWidth(), window.getHeight());
    m_postProcessPassFrameBuffer2.resize(window.getWidth(), window.getHeight());

    // Pass 1: fxaa
    m_postProcessPassFrameBuffer0.setActive(GL_FRAMEBUFFER);
    {
        ScopedTimer timer(profiler(), "post: fxaa");
        if (camera.getFeatureInfo().fxaaActive)
        {
            m_fxaaPass.render(window, manager, texture);
        }
        else
        {
            // Passthrough
            m_displayPass.render(window, manager, texture);
        }
    }

    // Pass 2: fog
    // TODO Fog parameter
    m_postProcessPassFrameBuffer1.setActive(GL_FRAMEBUFFER);
    {
        ScopedTimer timer(profiler(), "post: fog");
        m_fogPass.render(camera, window, manager, m_postProcessPassTexture0, depthTexture);
    }
    // Foggy scene in tex1

    // Pass 3: dof
	m_postProcessPassFrameBuffer0.setActive(GL_FRAMEBUFFER);
    if (camera.getFeatureInfo().dofActive)
    {
        // Pass 3.1: gauss blur
        {
            ScopedTimer timer(profiler(), "post: blur");
            m_gaussBlurPass.renderVertical(window, manager, m_postProcessPassTexture1);
            // Blurred in tex0

            m_postProcessPassFrameBuffer2.setActive(GL_FRAMEBUFFER);
            m_gaussBlurPass.renderHorizontal(window, manager, m_postProcessPassTexture0);
            // Blurred in tex 2
        }

        // TODO DOF parameter
        m_postProcessPassFrameBuffer0.setActive(GL_FRAMEBUFFER);
        {
            ScopedTimer timer(profiler(), "post: depth of field");
            m_depthOfFieldPass.render(camera, window, manager, m_postProcessPassTexture1,
                                      m_postProcessPassTexture2, depthTexture,
                                      m_transformer.getInverseViewProjectionMatrix());
        }
    }
    else
    {
        // Passthrough
        m_displayPass.render(window, manager, m_postProcessPassTexture1);
    }
    // Processed scene in tex 0

	m_postProcessPassFrameBuffer2.setActive(GL_FRAMEBUFFER);
    m_displayPass.render(window, manager, m_postProcessPassTexture0);

    // Set output texture
    m_postProcessPassOutputTexture = m_postProcessPassTexture2;
	return;
}

bool PostProcessPass::init(IResourceManager* manager)
{
    if (!m_fxaaPass.init(manager))
    {
        spdlog::error("Failed to initialize fxaa pass.");
        return false;
    }

    if (!m_fogPass.init(manager))
    {
        spdlog::error("Failed to initialize fog pass.");
        return false;
    }

    if (!m_gaussBlurPass.init(manager))
    {
        spdlog::error("Failed to initialize gauss blur pass.");
        return false;
    }

    if (!m_depthOfFieldPass.init(manager))
    {
        spdlog::error("Failed to initialize depth of field pass.");
        return false;
    }

    if (!m_displayPass.init(manager))
    {
        spdlog::error("Failed to initialize display pass.");
        return false;
    }

    // Init post processing FBOs
    // Texture 0
    m_postProcessPassTexture0 = std::make_shared<Texture>();
    if (!m_postProcessPassTexture0->init(800, 600, GL_RGBA16F))
    {
        spdlog::error("Failed to initialize post process pass texture 0.");
        return false;
    }

    // Texture 1
    m_postProcessPassTexture1 = std::make_shared<Texture>();
    if (!m_postProcessPassTexture1->init(800, 600, GL_RGBA16F))
    {
        spdlog::error("Failed to initialize post process pass texture 1.");
        return false;
    }

    // Texture 2
    m_postProcessPassTexture2 = std::make_shared<Texture>();
    if (!m_postProcessPassTexture2->init(800, 600, GL_RGBA16F))
    {
        spdlog::error("Failed to initialize post process pass texture 2.");
        return false;
    }

    m_postProcessPassFrameBuffer0.attach(m_postProcessPassTexture0, GL_COLOR_ATTACHMENT0);
    m_postProcessPassFrameBuffer1.attach(m_postProcessPassTexture1, GL_COLOR_ATTACHMENT0);
    m_postProcessPassFrameBuffer2.attach(m_postProcessPassTexture2, GL_COLOR_ATTACHMENT0);
    // TODO Depth attachment required?

    return true;
}

const std::shared_ptr<Texture>& PostProcessPass::getOutputTexture() const
{
    return m_postProcessPassOutputTexture;
}
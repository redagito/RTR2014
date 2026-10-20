#include <spdlog/spdlog.h>
#include "graphics/renderer/ForwardRenderer.h"

#include <cassert>
#include <string>

#include "graphics/IScene.h"
#include "graphics/ISceneQuery.h"
#include "graphics/ICamera.h"
#include "graphics/IWindow.h"
#include "graphics/IGraphicsResourceManager.h"

#include "graphics/renderer/core/RendererCoreConfig.h"

#include "util/Profiler.h"

#include "resource/IResourceManager.h"


ForwardRenderer::ForwardRenderer()
{
    return;
}

ForwardRenderer::~ForwardRenderer()
{
    // TODO Cleanup
}

bool ForwardRenderer::init(IResourceManager* manager)
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
    return m_forwardRenderPass.init(manager);
}

void ForwardRenderer::draw(const IScene& scene, const ICamera& camera, const IWindow& window, const IGraphicsResourceManager& manager)
{
    // Draw init
	window.setActive();

    // Initializiation
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // Reset viewport
    glViewport(0, 0, window.getWidth(), window.getHeight());

    {
        ScopedTimer timer(profiler(), "forwardRenderPass");
        m_forwardRenderPass.render(scene, camera, window, manager);
    }

    // Post draw error check
}

std::unique_ptr<ForwardRenderer> ForwardRenderer::create(IResourceManager* manager)
{
    auto renderer = std::make_unique<ForwardRenderer>();
    if (!renderer->init(manager))
    {
        spdlog::error("Failed to create forward renderer, initialization failed.");
        return nullptr;
    }
    return renderer;
}

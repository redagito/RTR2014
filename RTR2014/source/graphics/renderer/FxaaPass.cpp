#include <spdlog/spdlog.h>
#include "graphics/renderer/FxaaPass.h"

#include <string>

#include "graphics/renderer/core/RendererCoreConfig.h"

#include "resource/IResourceManager.h"

#include "graphics/IWindow.h"
#include "graphics/IGraphicsResourceManager.h"
#include "graphics/Mesh.h"
#include "graphics/Texture.h"
#include "graphics/ShaderProgram.h"
#include "graphics/renderer/Renderer.h"

FxaaPass::FxaaPass()
{
    return;
}

FxaaPass::~FxaaPass()
{
    return;
}

void FxaaPass::render(const IWindow& window, const IGraphicsResourceManager& manager,
                      const std::shared_ptr<Texture>& texture)
{
    // Get fxaa shader
    ShaderProgram* fxaaShader = manager.getShaderProgram(m_fxaaPassShaderId);
    if (fxaaShader == nullptr)
    {
        spdlog::error("Shader program for fxaa pass could not be retrieved.");
        return;
    }
    fxaaShader->setActive();

    // Get screen space quad
    Mesh* quadMesh = manager.getMesh(m_screenQuadId);
    if (quadMesh == nullptr)
    {
        spdlog::error("Mesh object for fxaa pass could not be retrieved.");
        return;
    }

    // Set input texture
    texture->setActive(fxaaPassInputTextureUnit);
    fxaaShader->setUniform(sceneTextureUniformName, fxaaPassInputTextureUnit);

    // Set screen size
    fxaaShader->setUniform(screenWidthUniformName, (float)window.getWidth());
    fxaaShader->setUniform(screenHeightUniformName, (float)window.getHeight());

    Renderer::draw(quadMesh);
}

bool FxaaPass::init(IResourceManager* manager)
{
    // Fxaa shader
    std::string fxaaShaderFile = "data/shader/post/fxaa_pass.ini";
    m_fxaaPassShaderId = manager->loadShader(fxaaShaderFile);
    // Check if ok
    if (m_fxaaPassShaderId == invalidResource)
    {
        spdlog::error("Failed to initialize the shader from file {}.", fxaaShaderFile.c_str());
        return false;
    }

    // Screen quad mesh
    std::string quadMesh = "data/mesh/screen_quad.obj";
    m_screenQuadId = manager->loadMesh(quadMesh);
    if (m_screenQuadId == invalidResource)
    {
        spdlog::error("Failed to load screen quad mesh {}.", quadMesh.c_str());
        return false;
    }

    return true;
}
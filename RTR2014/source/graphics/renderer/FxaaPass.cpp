#include "graphics/renderer/FxaaPass.h"

#include <string>

#include "graphics/IGraphicsResourceManager.h"
#include "graphics/IWindow.h"
#include "graphics/Mesh.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Texture.h"
#include "graphics/renderer/Renderer.h"
#include "graphics/renderer/core/RendererCoreConfig.h"
#include "resource/IResourceManager.h"
#include "util/FatalError.h"

FxaaPass::FxaaPass() { return; }

FxaaPass::~FxaaPass() { return; }

void FxaaPass::render(const IWindow& window, const IGraphicsResourceManager& manager,
                      const std::shared_ptr<Texture>& texture)
{
    // Get fxaa shader
    ShaderProgram* fxaaShader = manager.getShaderProgram(m_fxaaPassShaderId);
    if (fxaaShader == nullptr)
    {
        fatalError("Shader program for fxaa pass could not be retrieved.");
    }
    fxaaShader->setActive();

    // Get screen space quad
    Mesh* quadMesh = manager.getMesh(m_screenQuadId);
    if (quadMesh == nullptr)
    {
        fatalError("Mesh object for fxaa pass could not be retrieved.");
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
        fatalError("Failed to initialize the shader from file {}.", fxaaShaderFile.c_str());
    }

    // Screen quad mesh
    std::string quadMesh = "data/mesh/screen_quad.obj";
    m_screenQuadId = manager->loadMesh(quadMesh);
    if (m_screenQuadId == invalidResource)
    {
        fatalError("Failed to load screen quad mesh {}.", quadMesh.c_str());
    }

    return true;
}
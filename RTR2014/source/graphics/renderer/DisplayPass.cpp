#include "graphics/renderer/DisplayPass.h"

#include <string>

#include "graphics/IGraphicsResourceManager.h"
#include "graphics/IWindow.h"
#include "graphics/Mesh.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Texture.h"
#include "graphics/renderer/FrameBuffer.h"
#include "graphics/renderer/Renderer.h"
#include "graphics/renderer/core/RendererCoreConfig.h"
#include "resource/IResourceManager.h"
#include "util/FatalError.h"

DisplayPass::DisplayPass() { return; }

DisplayPass::~DisplayPass() { return; }

void DisplayPass::render(const IWindow& window, const IGraphicsResourceManager& manager,
                         const std::shared_ptr<Texture>& texture)
{
    // Reset viewport
    glViewport(0, 0, window.getWidth(), window.getHeight());

    // Get display shader
    ShaderProgram* displayShader = manager.getShaderProgram(m_displayPassShaderId);
    if (displayShader == nullptr)
    {
        fatalError("Shader program for display pass could not be retrieved.");
    }
    displayShader->setActive();

    // Get screen space quad
    Mesh* quadMesh = manager.getMesh(m_screenQuadId);
    if (quadMesh == nullptr)
    {
        fatalError("Mesh object for illumination pass could not be retrieved.");
    }

    // Scene texture
    texture->setActive(displayPassSceneTextureUnit);
    displayShader->setUniform(sceneTextureUniformName, displayPassSceneTextureUnit);

    // Screen parameters
    displayShader->setUniform(screenWidthUniformName, (float)window.getWidth());
    displayShader->setUniform(screenHeightUniformName, (float)window.getHeight());

    Renderer::draw(quadMesh);
}

void DisplayPass::renderToScreen(const IWindow& window, const IGraphicsResourceManager& manager,
                                 const std::shared_ptr<Texture>& texture)
{
    // Set main FBO active
    FrameBuffer::setDefaultActive();
    render(window, manager, texture);
}

bool DisplayPass::init(IResourceManager* manager)
{
    // Display shader
    std::string displayPassShaderFile = "data/shader/display_pass.ini";
    m_displayPassShaderId = manager->loadShader(displayPassShaderFile);
    // Check if ok
    if (m_displayPassShaderId == invalidResource)
    {
        fatalError("Failed to initialize the shader from file {}.", displayPassShaderFile.c_str());
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
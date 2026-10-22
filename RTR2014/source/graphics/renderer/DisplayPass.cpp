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
                         const std::shared_ptr<Texture>& texture, bool toneMap)
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

    // The scene texture
    texture->setActive(displayPassSceneTextureUnit);
    displayShader->setUniform(sceneTextureUniformName, displayPassSceneTextureUnit);

    // Screen parameters
    displayShader->setUniform(screenWidthUniformName, (float)window.getWidth());
    displayShader->setUniform(screenHeightUniformName, (float)window.getHeight());

    // The radiance is mapped to the display range, the debug views show the
    // raw data of the buffer they display
    displayShader->setUniform(tonemapEnabledUniformName, toneMap ? 1 : 0);

    // The image covers the whole screen, it is not hidden by anything.
    glDisable(GL_DEPTH_TEST);

    Renderer::draw(quadMesh);
}

void DisplayPass::renderToScreen(const IWindow& window, const IGraphicsResourceManager& manager,
                                 const std::shared_ptr<Texture>& texture, bool toneMap)
{
    // Set main FBO active
    FrameBuffer::setDefaultActive();
    render(window, manager, texture, toneMap);
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
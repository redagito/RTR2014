#include <spdlog/spdlog.h>
#include "graphics/renderer/DepthVisualizationPass.h"

#include <string>

#include "graphics/renderer/core/RendererCoreConfig.h"

#include "resource/IResourceManager.h"

#include "graphics/IWindow.h"
#include "graphics/IGraphicsResourceManager.h"
#include "graphics/Mesh.h"
#include "graphics/Texture.h"
#include "graphics/ShaderProgram.h"
#include "graphics/renderer/Renderer.h"

DepthVisualizationPass::DepthVisualizationPass()
{
    return;
}

DepthVisualizationPass::~DepthVisualizationPass()
{
    return;
}

void DepthVisualizationPass::render(const IWindow& window, const IGraphicsResourceManager& manager,
                                    const std::shared_ptr<Texture>& depthTexture, float zNear,
                                    float zFar)
{
    // The pass draws to the screen, the viewport has to match it.
    glViewport(0, 0, window.getWidth(), window.getHeight());

    // Get shader
    ShaderProgram* shader = manager.getShaderProgram(m_visualizeDepthPassShaderId);
    if (shader == nullptr)
    {
        spdlog::error("Shader program for depth visualization pass could not be retrieved.");
        return;
    }
    shader->setActive();

    // Get screen space quad
    Mesh* quadMesh = manager.getMesh(m_screenQuadId);
    if (quadMesh == nullptr)
    {
        spdlog::error("Mesh object for depth visualization pass could not be retrieved.");
        return;
    }

    // Depth texture
    depthTexture->setActive(visualizeDepthPassDepthTextureUnit);
    shader->setUniform(depthTextureUniformName, visualizeDepthPassDepthTextureUnit);

    /// Screen size
    shader->setUniform(screenWidthUniformName, (float)window.getWidth());
    shader->setUniform(screenHeightUniformName, (float)window.getHeight());

    // camera parameters
    shader->setUniform(cameraZNearUniformName, zNear);
    shader->setUniform(cameraZFarUniformName, zFar);

    // Perform pass
    Renderer::draw(quadMesh);
}

bool DepthVisualizationPass::init(IResourceManager* manager)
{
    // Get shader
    std::string shaderFile = "data/shader/debug/visualize_depth_buffer_pass.ini";
    m_visualizeDepthPassShaderId = manager->loadShader(shaderFile);
    // Check if ok
    if (m_visualizeDepthPassShaderId == invalidResource)
    {
        spdlog::error("Failed to initialize the shader from file {}.", shaderFile.c_str());
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
#include <spdlog/spdlog.h>
#include "graphics/renderer/IlluminationPass.h"

#include <string>

#include "graphics/renderer/core/RendererCoreConfig.h"

#include "resource/IResourceManager.h"

#include "graphics/IWindow.h"
#include "graphics/IGraphicsResourceManager.h"
#include "graphics/Mesh.h"
#include "graphics/Texture.h"
#include "graphics/ShaderProgram.h"
#include "graphics/renderer/Renderer.h"

IlluminationPass::IlluminationPass()
{
    return;
}

IlluminationPass::~IlluminationPass()
{
    return;
}

void IlluminationPass::render(const IWindow& window, const IGraphicsResourceManager& manager,
                              const std::shared_ptr<Texture>& lightTexture,
                              const std::shared_ptr<Texture>& diffuseGlowTexture)
{
    // Reset viewport
    glViewport(0, 0, window.getWidth(), window.getHeight());
    m_illumationPassFrameBuffer.resize(window.getWidth(), window.getHeight());
    // TODO Clear illumination pass buffer?

    // Get illumination shader
    ShaderProgram* illuminationShader = manager.getShaderProgram(m_illuminationPassShaderId);
    if (illuminationShader == nullptr)
    {
        spdlog::error("Shader program for illumination pass could not be retrieved.");
        return;
    }
    illuminationShader->setActive();

    // Get screen space quad
    Mesh* quadMesh = manager.getMesh(m_illuminationPassScreenQuadId);
    if (quadMesh == nullptr)
    {
        spdlog::error("Mesh object for illumination pass could not be retrieved.");
        return;
    }

    // Set textures
    // L-buffer light texture
    lightTexture->setActive(illuminationPassLightTextureUnit);
    illuminationShader->setUniform(lightTextureUniformName, illuminationPassLightTextureUnit);

    // G-buffer diffuse glow texture
    diffuseGlowTexture->setActive(illuminationPassDiffuseGlowTextureUnit);
    illuminationShader->setUniform(diffuseGlowTextureUniformName,
                                   illuminationPassDiffuseGlowTextureUnit);

    // Screen parameters
    illuminationShader->setUniform(screenWidthUniformName, (float)window.getWidth());
    illuminationShader->setUniform(screenHeightUniformName, (float)window.getHeight());

    // Draw into frame buffer
    m_illumationPassFrameBuffer.setActive(GL_FRAMEBUFFER);
    Renderer::draw(quadMesh);
    m_illumationPassFrameBuffer.setInactive(GL_FRAMEBUFFER);
}

bool IlluminationPass::init(IResourceManager* manager)
{
    // Load illumination shader
    std::string illuminationPassShaderFile = "data/shader/deferred/illumination_pass.ini";
    m_illuminationPassShaderId = manager->loadShader(illuminationPassShaderFile);
    // Check if ok
    if (m_illuminationPassShaderId == invalidResource)
    {
        spdlog::error("Failed to initialize the shader from file {}.",
                   illuminationPassShaderFile.c_str());
        return false;
    }


    // Screen quad mesh
    std::string quadMesh = "data/mesh/screen_quad.obj";
    m_illuminationPassScreenQuadId = manager->loadMesh(quadMesh);
    // Check if ok
    if (m_illuminationPassScreenQuadId == invalidResource)
    {
        spdlog::error("Failed to load screen quad mesh {}.", quadMesh.c_str());
        return false;
    }


    // FBO
    m_illuminationPassTexture = std::make_shared<Texture>();
    if (!m_illuminationPassTexture->init(800, 600, GL_RGB))
    {
        spdlog::error("Failed to initialize illumination pass texture.");
        return false;
    }
    m_illumationPassFrameBuffer.attach(m_illuminationPassTexture, GL_COLOR_ATTACHMENT0);

    // TODO Check FBO


    return true;
}

const std::shared_ptr<Texture>& IlluminationPass::getOutputTexture() const
{
    return m_illuminationPassTexture;
}
#include <spdlog/spdlog.h>
#include "graphics/renderer/DeferredRenderer.h"

#include <string>

#include "graphics/renderer/core/RendererCoreConfig.h"

#include "resource/IResourceManager.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "graphics/ICamera.h"
#include "graphics/IWindow.h"
#include "graphics/IGraphicsResourceManager.h"
#include "graphics/Mesh.h"
#include "graphics/Texture.h"
#include "graphics/ShaderProgram.h"
#include "graphics/camera/TransformUtils.h"
#include "graphics/renderer/FrameBuffer.h"
#include "graphics/renderer/RenderBuffer.h"


void DeferredRenderer::postProcessPass(const ICamera& camera, const IWindow& window,
                                        const IGraphicsResourceManager& manager,
                                        const std::shared_ptr<Texture>& texture)
{
    // Reset viewport
    glViewport(0, 0, window.getWidth(), window.getHeight());
    // Resize frame buffer
    m_postProcessPassFrameBuffer0.resize(window.getWidth(), window.getHeight());
    m_postProcessPassFrameBuffer1.resize(window.getWidth(), window.getHeight());
    m_postProcessPassFrameBuffer2.resize(window.getWidth(), window.getHeight());

    // Pass 1: fxaa
    m_postProcessPassFrameBuffer0.setActive(GL_FRAMEBUFFER);
    if (camera.getFeatureInfo().fxaaActive)
    {
        fxaaPass(window, manager, texture);
    }
    else
    {
        // Passthrough
        passthroughPass(window, manager, texture);
    }

    // Pass 2: fog
    // TODO Fog parameter
    m_postProcessPassFrameBuffer1.setActive(GL_FRAMEBUFFER);
    fogPass(camera, window, manager, m_postProcessPassTexture0);
    // Foggy scene in tex1

    // Pass 3: dof
	m_postProcessPassFrameBuffer0.setActive(GL_FRAMEBUFFER);
    if (camera.getFeatureInfo().dofActive)
    {
        // Pass 3.1: gauss blur
        gaussBlurVerticalPass(window, manager, m_postProcessPassTexture1);
        // Blurred in tex0

        m_postProcessPassFrameBuffer2.setActive(GL_FRAMEBUFFER);
        gaussBlurHorizontalPass(window, manager, m_postProcessPassTexture0);
        // Blurred in tex 2

        // TODO DOF parameter
        m_postProcessPassFrameBuffer0.setActive(GL_FRAMEBUFFER);
        depthOfFieldPass(camera, window, manager, m_postProcessPassTexture1,
                         m_postProcessPassTexture2);
    }
    else
    {
        // Passthrough
        passthroughPass(window, manager, m_postProcessPassTexture1);
    }
    // Processed scene in tex 0

	m_postProcessPassFrameBuffer2.setActive(GL_FRAMEBUFFER);
    passthroughPass(window, manager, m_postProcessPassTexture0);

    // Set output texture
    m_postProcessPassOutputTexture = m_postProcessPassTexture2;
	return;
}

void DeferredRenderer::fxaaPass(const IWindow& window, const IGraphicsResourceManager& manager,
                                 const std::shared_ptr<Texture>& texture)
{
    // Get fxaa shader
    ShaderProgram* fxaaShader = manager.getShaderProgram(m_fxaaPassShaderId);
    if (fxaaShader == nullptr)
    {
        spdlog::error("Shader program for fxaa pass could not be retrieved.");
        return;
    }

    // Get screen space quad
    Mesh* quadMesh = manager.getMesh(m_postProcessScreenQuadId);
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

void DeferredRenderer::fogPass(const ICamera& camera, const IWindow& window,
                                const IGraphicsResourceManager& manager,
                                const std::shared_ptr<Texture>& texture)
{
    // Get fog shader
    ShaderProgram* fogShader = manager.getShaderProgram(m_fogPassShaderId);
    if (fogShader == nullptr)
    {
        spdlog::error("Shader program for fog pass could not be retrieved.");
        return;
    }

    // Get screen space quad
    Mesh* quadMesh = manager.getMesh(m_postProcessScreenQuadId);
    if (quadMesh == nullptr)
    {
        spdlog::error("Mesh object for fog pass could not be retrieved.");
        return;
    }

    fogShader->setActive();

    texture->setActive(fogPassSceneTextureUnit);
    fogShader->setUniform(sceneTextureUniformName, fogPassSceneTextureUnit);

    m_geometryPass.getDepthTexture()->setActive(fogPassDepthTextureUnit);
    fogShader->setUniform(depthTextureUniformName, fogPassDepthTextureUnit);

    fogShader->setUniform(screenWidthUniformName, (float)window.getWidth());
    fogShader->setUniform(screenHeightUniformName, (float)window.getHeight());

    int fogType = 0;
    switch (camera.getFeatureInfo().fogType)
    {
    case FogType::None:
        fogType = 0;
        break;
    case FogType::Linear:
        fogType = 1;
        break;
    case FogType::Exp:
        fogType = 2;
        break;
    case FogType::Exp2:
        fogType = 3;
        break;
    default:
        fogType = 0;
    }
    fogShader->setUniform(fogPassTypeUniformName, fogType);

    Renderer::draw(quadMesh);
}

void DeferredRenderer::gaussBlurVerticalPass(const IWindow& window,
                                              const IGraphicsResourceManager& manager,
                                              const std::shared_ptr<Texture>& texture)
{
    // Get gauss shader
    ShaderProgram* shader = manager.getShaderProgram(m_gaussBlurVerticalShaderId);
    if (shader == nullptr)
    {
        spdlog::error("Shader program for vertical gauss blur pass could not be retrieved.");
        return;
    }

    // Get screen space quad
    Mesh* quadMesh = manager.getMesh(m_postProcessScreenQuadId);
    if (quadMesh == nullptr)
    {
        spdlog::error("Mesh object for vertical gauss blur pass could not be retrieved.");
        return;
    }

    // Input texture
    texture->setActive(gaussBlurVerticalPassInputTextureUnit);
    shader->setUniform(sceneTextureUniformName, gaussBlurVerticalPassInputTextureUnit);

    // Blur parameter
    shader->setUniform(blurStrengthUniformName, 3.f);

    /// Screen size
    shader->setUniform(screenWidthUniformName, (float)window.getWidth());
    shader->setUniform(screenHeightUniformName, (float)window.getHeight());

    // Perform pass
    Renderer::draw(quadMesh);
}

void DeferredRenderer::gaussBlurHorizontalPass(const IWindow& window,
                                                const IGraphicsResourceManager& manager,
                                                const std::shared_ptr<Texture>& texture)
{
    // Get gauss shader
    ShaderProgram* shader = manager.getShaderProgram(m_gaussBlurHorizontalShaderId);
    if (shader == nullptr)
    {
        spdlog::error("Shader program for horizontal gauss blur pass could not be retrieved.");
        return;
    }

    // Get screen space quad
    Mesh* quadMesh = manager.getMesh(m_postProcessScreenQuadId);
    if (quadMesh == nullptr)
    {
        spdlog::error("Mesh object for horizontal gauss blur pass could not be retrieved.");
        return;
    }

    // Input texture
    texture->setActive(gaussBlurHoriontalPassInputTextureUnit);
    shader->setUniform(sceneTextureUniformName, gaussBlurHoriontalPassInputTextureUnit);

    // Blur parameter
    shader->setUniform(blurStrengthUniformName, 3.f);

    /// Screen size
    shader->setUniform(screenWidthUniformName, (float)window.getWidth());
    shader->setUniform(screenHeightUniformName, (float)window.getHeight());

    // Perform pass
    Renderer::draw(quadMesh);
}

void DeferredRenderer::depthOfFieldPass(const ICamera& camera, const IWindow& window,
                                         const IGraphicsResourceManager& manager,
                                         const std::shared_ptr<Texture>& sceneTexture,
                                         const std::shared_ptr<Texture>& blurTexture)
{
    // Get shader
    ShaderProgram* shader = manager.getShaderProgram(m_depthOfFieldPassShaderId);
    if (shader == nullptr)
    {
        spdlog::error("Shader program for depth-of-field pass could not be retrieved.");
        return;
    }

    // Get screen space quad
    Mesh* quadMesh = manager.getMesh(m_postProcessScreenQuadId);
    if (quadMesh == nullptr)
    {
        spdlog::error("Mesh object for depth-of-field pass could not be retrieved.");
        return;
    }

    // Input scene texture
    sceneTexture->setActive(depthOfFieldPassSceneTextureUnit);
    shader->setUniform(sceneTextureUniformName, depthOfFieldPassSceneTextureUnit);

    // Input blur texture
    blurTexture->setActive(depthOfFieldPassBlurTextureUnit);
    shader->setUniform(blurTextureUniformName, depthOfFieldPassBlurTextureUnit);

    // Input depth texture
    m_geometryPass.getDepthTexture()->setActive(depthOfFieldPassDepthTextureUnit);
    shader->setUniform(depthTextureUniformName, depthOfFieldPassDepthTextureUnit);

    // Depth-of-field parameters
    shader->setUniform(blurNearUniformName, camera.getFeatureInfo().dofNearBlur);
    shader->setUniform(focusNearUniformName, camera.getFeatureInfo().dofNearFocus);
    shader->setUniform(focusFarUniformName, camera.getFeatureInfo().dofFarFocus);
    shader->setUniform(blurFarUniformName, camera.getFeatureInfo().dofFarBlur);

    // Camera position
    shader->setUniform(cameraPositionUniformName, camera.getPosition());

    // Transformation
    shader->setUniform(inverseViewProjectionMatrixUniformName,
                       m_transformer.getInverseViewProjectionMatrix());

    /// Screen size
    shader->setUniform(screenWidthUniformName, (float)window.getWidth());
    shader->setUniform(screenHeightUniformName, (float)window.getHeight());

    // Perform pass
    Renderer::draw(quadMesh);
}

void DeferredRenderer::displayPass(const IWindow& window, const IGraphicsResourceManager& manager,
                                    const std::shared_ptr<Texture>& texture)
{
    // Set main FBO active
    FrameBuffer::setDefaultActive();
    passthroughPass(window, manager, texture);
}

void DeferredRenderer::passthroughPass(const IWindow& window,
                                        const IGraphicsResourceManager& manager,
                                        const std::shared_ptr<Texture>& texture)
{
    // Reset viewport
    glViewport(0, 0, window.getWidth(), window.getHeight());

    // Get display shader
    ShaderProgram* displayShader = manager.getShaderProgram(m_displayPassShaderId);
    if (displayShader == nullptr)
    {
        spdlog::error("Shader program for display pass could not be retrieved.");
        return;
    }

    // Get screen space quad
    Mesh* quadMesh = manager.getMesh(m_illuminationPassScreenQuadId);
    if (quadMesh == nullptr)
    {
        spdlog::error("Mesh object for illumination pass could not be retrieved.");
        return;
    }

    // Scene texture
    texture->setActive(displayPassSceneTextureUnit);
    displayShader->setUniform(sceneTextureUniformName, displayPassSceneTextureUnit);

    // Screen parameters
    displayShader->setUniform(screenWidthUniformName, (float)window.getWidth());
    displayShader->setUniform(screenHeightUniformName, (float)window.getHeight());

    Renderer::draw(quadMesh);
}

void DeferredRenderer::visualizeDepthPass(const ICamera& camera, const IWindow& window,
                                           const IGraphicsResourceManager& manager)
{
    // Get shader
    ShaderProgram* shader = manager.getShaderProgram(m_visualizeDepthPassShaderId);
    if (shader == nullptr)
    {
        spdlog::error("Shader program for depth visualization pass could not be retrieved.");
        return;
    }

    // Get screen space quad
    Mesh* quadMesh = manager.getMesh(m_postProcessScreenQuadId);
    if (quadMesh == nullptr)
    {
        spdlog::error("Mesh object for depth visualization pass could not be retrieved.");
        return;
    }

    // Depth texture
    m_geometryPass.getDepthTexture()->setActive(visualizeDepthPassDepthTextureUnit);
    shader->setUniform(depthTextureUniformName, visualizeDepthPassDepthTextureUnit);

    /// Screen size
    shader->setUniform(screenWidthUniformName, (float)window.getWidth());
    shader->setUniform(screenHeightUniformName, (float)window.getHeight());

    // camera parameters
    // TODO Replace
    shader->setUniform(cameraZNearUniformName, 0.01f);
    shader->setUniform(cameraZFarUniformName, 1000.f);

    // Perform pass
    Renderer::draw(quadMesh);
}

bool DeferredRenderer::initPostProcessPass(IResourceManager* manager)
{
    // Gauss blur pass
    if (!initGaussBlurHorizontalPass(manager))
    {
        spdlog::error("Failed to initialize gauss blur horizontal pass.");
        return false;
    }
    if (!initGaussBlurVerticalPass(manager))
    {
        spdlog::error("Failed to initialize gauss blur vertical pass.");
        return false;
    }

    // FXAA
    if (!initFxaaPass(manager))
    {
        spdlog::error("Failed to initialize fxaa pass.");
        return false;
    }

    // Fog
    if (!initFogPass(manager))
    {
        spdlog::error("Failed to initialize fog pass.");
        return false;
    }

    // Screen quad mesh
    std::string quadMesh = "data/mesh/screen_quad.obj";
    m_postProcessScreenQuadId = manager->loadMesh(quadMesh);
    if (m_postProcessScreenQuadId == invalidResource)
    {
        spdlog::error("Failed to load screen quad mesh {}.", quadMesh.c_str());
        return false;
    }

    // Init post processing FBOs
    // Texture 0
    m_postProcessPassTexture0 = std::make_shared<Texture>();
    if (!m_postProcessPassTexture0->init(800, 600, GL_RGB))
    {
        spdlog::error("Failed to initialize post process pass texture 0.");
        return false;
    }

    // Texture 1
    m_postProcessPassTexture1 = std::make_shared<Texture>();
    if (!m_postProcessPassTexture1->init(800, 600, GL_RGB))
    {
        spdlog::error("Failed to initialize post process pass texture 1.");
        return false;
    }

    // Texture 2
    m_postProcessPassTexture2 = std::make_shared<Texture>();
    if (!m_postProcessPassTexture2->init(800, 600, GL_RGB))
    {
        spdlog::error("Failed to initialize post process pass texture 2.");
        return false;
    }

    m_postProcessPassFrameBuffer0.attach(m_postProcessPassTexture0, GL_COLOR_ATTACHMENT0);
    m_postProcessPassFrameBuffer1.attach(m_postProcessPassTexture1, GL_COLOR_ATTACHMENT0);
    m_postProcessPassFrameBuffer2.attach(m_postProcessPassTexture2, GL_COLOR_ATTACHMENT0);
    // TODO Depth attachment required?

    if (!initDepthOfFieldPass(manager))
    {
        spdlog::error("Failed to initialize depth of field pass.");
        return false;
    }
    return true;
}

bool DeferredRenderer::initDepthOfFieldPass(IResourceManager* manager)
{
    // Depth of field shader
    std::string depthOfFieldShaderFile = "data/shader/post/depth_of_field_pass.ini";
    m_depthOfFieldPassShaderId = manager->loadShader(depthOfFieldShaderFile);
    // Check if ok
    if (m_depthOfFieldPassShaderId == invalidResource)
    {
        spdlog::error("Failed to initialize the shader from file {}.", depthOfFieldShaderFile.c_str());
        return false;
    }
    return true;
}

bool DeferredRenderer::initGaussBlurVerticalPass(IResourceManager* manager)
{
    // Depth of field shader
    std::string gaussBlurVerticalShaderFile = "data/shader/post/gauss_blur_vertical_pass.ini";
    m_gaussBlurVerticalShaderId = manager->loadShader(gaussBlurVerticalShaderFile);
    // Check if ok
    if (m_gaussBlurVerticalShaderId == invalidResource)
    {
        spdlog::error("Failed to initialize the shader from file {}.",
                  gaussBlurVerticalShaderFile.c_str());
        return false;
    }
    return true;
}

bool DeferredRenderer::initGaussBlurHorizontalPass(IResourceManager* manager)
{
    // Depth of field shader
    std::string gaussBlurHorizontalShaderFile = "data/shader/post/gauss_blur_horizontal_pass.ini";
    m_gaussBlurHorizontalShaderId = manager->loadShader(gaussBlurHorizontalShaderFile);
    // Check if ok
    if (m_gaussBlurHorizontalShaderId == invalidResource)
    {
        spdlog::error("Failed to initialize the shader from file {}.",
                  gaussBlurHorizontalShaderFile.c_str());
        return false;
    }
    return true;
}

bool DeferredRenderer::initFxaaPass(IResourceManager* manager)
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
    return true;
}

bool DeferredRenderer::initFogPass(IResourceManager* manager)
{
    // Fog shader
    std::string fogShaderFile = "data/shader/post/fog_pass.ini";
    m_fogPassShaderId = manager->loadShader(fogShaderFile);
    // Check if ok
    if (m_fogPassShaderId == invalidResource)
    {
        spdlog::error("Failed to initialize the shader from file {}.", fogShaderFile.c_str());
        return false;
    }
    return true;
}

bool DeferredRenderer::initDisplayPass(IResourceManager* manager)
{
    // Display shader
    std::string displayPassShaderFile = "data/shader/display_pass.ini";
    m_displayPassShaderId = manager->loadShader(displayPassShaderFile);
    // Check if ok
    if (m_displayPassShaderId == invalidResource)
    {
        spdlog::error("Failed to initialize the shader from file {}.", displayPassShaderFile.c_str());
        return false;
    }
    return true;
}

bool DeferredRenderer::initVisualizeDepthPass(IResourceManager* manager)
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
    return true;
}


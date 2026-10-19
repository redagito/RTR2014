#include <spdlog/spdlog.h>
#include "graphics/renderer/DepthOfFieldPass.h"

#include <string>

#include "graphics/renderer/core/RendererCoreConfig.h"

#include "resource/IResourceManager.h"

#include "graphics/ICamera.h"
#include "graphics/IWindow.h"
#include "graphics/IGraphicsResourceManager.h"
#include "graphics/Mesh.h"
#include "graphics/Texture.h"
#include "graphics/ShaderProgram.h"
#include "graphics/renderer/Renderer.h"

DepthOfFieldPass::DepthOfFieldPass()
{
    return;
}

DepthOfFieldPass::~DepthOfFieldPass()
{
    return;
}

void DepthOfFieldPass::render(const ICamera& camera, const IWindow& window,
                              const IGraphicsResourceManager& manager,
                              const std::shared_ptr<Texture>& sceneTexture,
                              const std::shared_ptr<Texture>& blurTexture,
                              const std::shared_ptr<Texture>& depthTexture,
                              const glm::mat4& inverseViewProjection)
{
    // Get shader
    ShaderProgram* shader = manager.getShaderProgram(m_depthOfFieldPassShaderId);
    if (shader == nullptr)
    {
        spdlog::error("Shader program for depth-of-field pass could not be retrieved.");
        return;
    }

    // Get screen space quad
    Mesh* quadMesh = manager.getMesh(m_screenQuadId);
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
    depthTexture->setActive(depthOfFieldPassDepthTextureUnit);
    shader->setUniform(depthTextureUniformName, depthOfFieldPassDepthTextureUnit);

    // Depth-of-field parameters
    shader->setUniform(blurNearUniformName, camera.getFeatureInfo().dofNearBlur);
    shader->setUniform(focusNearUniformName, camera.getFeatureInfo().dofNearFocus);
    shader->setUniform(focusFarUniformName, camera.getFeatureInfo().dofFarFocus);
    shader->setUniform(blurFarUniformName, camera.getFeatureInfo().dofFarBlur);

    // Camera position
    shader->setUniform(cameraPositionUniformName, camera.getPosition());

    // Transformation
    shader->setUniform(inverseViewProjectionMatrixUniformName, inverseViewProjection);

    /// Screen size
    shader->setUniform(screenWidthUniformName, (float)window.getWidth());
    shader->setUniform(screenHeightUniformName, (float)window.getHeight());

    // Perform pass
    Renderer::draw(quadMesh);
}

bool DepthOfFieldPass::init(IResourceManager* manager)
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
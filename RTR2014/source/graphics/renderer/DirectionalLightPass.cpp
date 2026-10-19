#include <spdlog/spdlog.h>
#include "graphics/renderer/DirectionalLightPass.h"

#include <string>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "graphics/renderer/core/RendererCoreConfig.h"

#include "resource/IResourceManager.h"

#include "graphics/IScene.h"
#include "graphics/ISceneQuery.h"
#include "graphics/ICamera.h"
#include "graphics/IWindow.h"
#include "graphics/IGraphicsResourceManager.h"
#include "graphics/Mesh.h"
#include "graphics/Texture.h"
#include "graphics/ShaderProgram.h"
#include "graphics/camera/StaticCamera.h"
#include "graphics/renderer/Renderer.h"

DirectionalLightPass::DirectionalLightPass()
{
    return;
}

DirectionalLightPass::~DirectionalLightPass()
{
    return;
}

void DirectionalLightPass::render(const IScene& scene, const ICamera& camera,
                                  const IWindow& window, const IGraphicsResourceManager& manager,
                                  ISceneQuery& query, FrameBuffer& lightBuffer,
                                  const std::shared_ptr<Texture>& depthTexture,
                                  const std::shared_ptr<Texture>& normalSpecularTexture)
{
    // Restrieve shader
    ShaderProgram* directionalLightPassShader =
        manager.getShaderProgram(m_directionalLightPassShaderId);
    if (directionalLightPassShader == nullptr)
    {
        spdlog::error("Shader program for directional light pass could not be retrieved.");
        return;
    }

    // Retrieve fullscreen quad mesh
    Mesh* quadMesh = manager.getMesh(m_directionalLightScreenQuadId);
    if (quadMesh == nullptr)
    {
        spdlog::error("Mesh object for directional light pass could not be retrieved.");
        return;
    }

    // Set view and projection matrices
    m_transformer.setViewMatrix(camera.getView());
    m_transformer.setProjectionMatrix(camera.getProjection());

    // Render point light volumes into light buffer
    while (query.hasNextDirectionalLight())
    {
        // Retrieve light id
        SceneObjectId directionalLightId = query.getNextDirectionalLight();
        // Retrieve light parameters
        glm::vec3 direction;
        glm::vec3 color;
        float intensity;
        bool castsShadow;

        if (!scene.getDirectionalLight(directionalLightId, direction, color, intensity,
                                       castsShadow))
        {
            spdlog::error("Failed to retrieve directional light data from point light id {}.",
                      directionalLightId);
        }
        else
        {
            // Create shadow camera
            glm::mat4 shadowView = glm::lookAt(glm::vec3(0), glm::normalize(direction),
                                               glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 shadowProj = glm::ortho(-150.0f, 150.0f, -150.0f, 150.0f, -150.0f, 150.0f);
            StaticCamera shadowCamera = StaticCamera(shadowView, shadowProj, camera.getPosition());

            // Render shadow map
            if (castsShadow)
            {
                m_shadowMapPass.render(scene, shadowCamera, window, manager);
            }

            // Prepare light pass frame buffer
            lightBuffer.setActive(GL_FRAMEBUFFER);
            glViewport(0, 0, window.getWidth(), window.getHeight());

            // No depth testing for light volumes
            glDisable(GL_DEPTH_TEST);
            // Additive blending for light accumulation
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);

            // Reset culling
            glCullFace(GL_BACK);

            // Set shader active
            directionalLightPassShader->setActive();

            // Set textures for point light pass
            // Set depth texture
            depthTexture->setActive(lightPassDepthTextureUnit);
            directionalLightPassShader->setUniform(depthTextureUniformName,
                                                   lightPassDepthTextureUnit);

            // Set texture with world space normal and specular power
            normalSpecularTexture->setActive(lightPassNormalSpecularTextureUnit);
            directionalLightPassShader->setUniform(normalSpecularTextureUniformName,
                                                   lightPassNormalSpecularTextureUnit);

            // Set shadow texture for shadow mapping
            m_shadowMapPass.getShadowMap()->setActive(lightPassShadowMapTextureUnit);
            directionalLightPassShader->setUniform(shadowMapTextureUniformName,
                                                   lightPassShadowMapTextureUnit);

            // Set screen size
            directionalLightPassShader->setUniform(screenWidthUniformName,
                                                   (float)window.getWidth());
            directionalLightPassShader->setUniform(screenHeightUniformName,
                                                   (float)window.getHeight());

            // Inverse view-projection
            directionalLightPassShader->setUniform(inverseViewProjectionMatrixUniformName,
                                                   m_transformer.getInverseViewProjectionMatrix());

            // Shadow ViewProjectionBias
            glm::mat4 shadowViewProjBiasMatrix =
                glm::mat4(0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f,
                          0.5f, 0.5f, 0.5f, 1.0f) *
                shadowProj * shadowView;
            directionalLightPassShader->setUniform(shadowViewProjectionBiasMatrixUniformName,
                                                   shadowViewProjBiasMatrix);

            // Set directional light parameters
            directionalLightPassShader->setUniform(lightDirectionUniformName, direction);
            directionalLightPassShader->setUniform(lightColorUniformName, color);
            directionalLightPassShader->setUniform(lightIntensityUniformName, intensity);
            Renderer::draw(quadMesh);
        }
    }

    // Reset culling
    glCullFace(GL_BACK);

    return;
}

bool DirectionalLightPass::init(IResourceManager* manager)
{
    // Uses same frame buffer as point light pass
    // Directional light shader
    std::string directionalLightPassShaderFile("data/shader/deferred/directional_light_pass.ini");
    m_directionalLightPassShaderId = manager->loadShader(directionalLightPassShaderFile);

    // Check if ok
    if (m_directionalLightPassShaderId == invalidResource)
    {
        spdlog::error("Failed to initialize the shader from file {}.",
                   directionalLightPassShaderFile.c_str());
        return false;
    }

    // Load quad mesh for directional light representation
    std::string quadMesh = "data/mesh/screen_quad.obj";
    m_directionalLightScreenQuadId = manager->loadMesh(quadMesh);
    if (m_directionalLightScreenQuadId == invalidResource)
    {
        spdlog::error("Failed to load screen quad mesh {}.", quadMesh.c_str());
        return false;
    }

    if (!m_shadowMapPass.init(manager))
    {
        spdlog::error("Failed to initialize shadow map pass.");
        return false;
    }

    return true;
}
#include <spdlog/spdlog.h>
#include "graphics/renderer/DeferredRenderer.h"

#include <string>

#include "graphics/renderer/core/RendererCoreConfig.h"

#include "resource/IResourceManager.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "graphics/IScene.h"
#include "graphics/ISceneQuery.h"
#include "graphics/ICamera.h"
#include "graphics/IWindow.h"
#include "graphics/IGraphicsResourceManager.h"
#include "graphics/Material.h"
#include "graphics/Mesh.h"
#include "graphics/Texture.h"
#include "graphics/ShaderProgram.h"
#include "graphics/camera/StaticCamera.h"
#include "graphics/renderer/RenderBuffer.h"
#include "graphics/renderer/Transformer.h"


void DeferredRenderer::lightPass(const IScene& scene, const ICamera& camera, const IWindow& window,
                                  const IGraphicsResourceManager& manager, ISceneQuery& query)
{
    // Prepare light pass frame buffer
    glViewport(0, 0, window.getWidth(), window.getHeight());
    // Resize
    m_lightPassFrameBuffer.resize(window.getWidth(), window.getHeight());
    // Enable light buffer
    m_lightPassFrameBuffer.setActive(GL_FRAMEBUFFER);

    // Retrieve ambient light
    glm::vec3 ambientColor;
    float ambientIntensity;
    scene.getAmbientLight(ambientColor, ambientIntensity);
    // Initialize light buffer with ambient light
    glClearColor(ambientColor.x * ambientIntensity, ambientColor.y * ambientIntensity,
                 ambientColor.z * ambientIntensity, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // No depth testing for light volumes
    glDisable(GL_DEPTH_TEST);
    // Additive blending for light accumulation
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    // Draw point light volumes
    pointLightPass(scene, camera, window, manager, query);

    // Draw directional lights
    directionalLightPass(scene, camera, window, manager, query);

    // Reset state and cleanup
    glDisable(GL_BLEND);
    m_lightPassFrameBuffer.setInactive(GL_FRAMEBUFFER);
}

void DeferredRenderer::pointLightPass(const IScene& scene, const ICamera& camera,
                                       const IWindow& window,
                                       const IGraphicsResourceManager& manager, ISceneQuery& query)
{
    // Point light pass
    ShaderProgram* pointLightPassShader = manager.getShaderProgram(m_pointLightPassShaderId);
    if (pointLightPassShader == nullptr)
    {
        spdlog::error("Shader program for point light pass could not be retrieved.");
        return;
    }

    Mesh* pointLightMesh = manager.getMesh(m_pointLightSphereId);
    if (pointLightMesh == nullptr)
    {
        spdlog::error("Mesh object for point light pass could not be retrieved.");
        return;
    }

    // Render point light volumes into light buffer
    while (query.hasNextPointLight())
    {
        // Retrieve light id
        SceneObjectId pointLightId = query.getNextPointLight();
        // Retrieve light parameters
        glm::vec3 position;
        glm::vec3 color;
        float intensity;
        float radius;
        bool castsShadow;

        if (!scene.getPointLight(pointLightId, position, radius, color, intensity, castsShadow))
        {
            spdlog::error("Failed to retrieve point light data from point light id {}.", pointLightId);
        }
        else
        {
            // fov should be 90.f instead of 89.54f, but this does not work, because the view is too
            // wide in this case. 89.54f is determined by testing. 89.53 is already too small and
            // 89.55 too big.
            if (castsShadow)
            {
                glm::mat4 shadowProj = glm::perspective(89.54f, 1.0f, 0.01f, radius * 1.5f);
                StaticCamera shadowCamera = StaticCamera(glm::mat4(), shadowProj, position);
                shadowCubePass(scene, shadowCamera, window, manager);
            }

            // Prepare light pass frame buffer
            glViewport(0, 0, window.getWidth(), window.getHeight());
            m_lightPassFrameBuffer.setActive(GL_FRAMEBUFFER);

            // No depth testing for light volumes
            glDisable(GL_DEPTH_TEST);
            // Additive blending for light accumulation
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);

            // Cull front facing faces
            glCullFace(GL_FRONT);

            // Set textures for point light pass
            // Set depth texture
            m_depthTexture->setActive(lightPassDepthTextureUnit);
            pointLightPassShader->setUniform(depthTextureUniformName, lightPassDepthTextureUnit);

            // Set texture with world space normal and specular power
            m_normalSpecularTexture->setActive(lightPassNormalSpecularTextureUnit);
            pointLightPassShader->setUniform(normalSpecularTextureUniformName,
                                             lightPassNormalSpecularTextureUnit);

            // Set shadow texture for shadow mapping
            glActiveTexture(GL_TEXTURE0 + lightPassShadowMapTextureUnit);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowCubeTexture->getId());
            pointLightPassShader->setUniform(shadowCubeTextureUniformName,
                                             lightPassShadowMapTextureUnit);

            // Set screen size
            pointLightPassShader->setUniform(screenWidthUniformName, (float)window.getWidth());
            pointLightPassShader->setUniform(screenHeightUniformName, (float)window.getHeight());

			// Camera position
			pointLightPassShader->setUniform(cameraPositionUniformName, camera.getPosition());

            // Inverse view-projection
            pointLightPassShader->setUniform(inverseViewProjectionMatrixUniformName,
                                             m_transformer.getInverseViewProjectionMatrix());

            m_transformer.setPosition(position);
            // Scale is calculated from light radius
            // Sphere model has radius 1.f
            m_transformer.setScale(glm::vec3(radius));
            // Point lights do not have rotation
            m_transformer.setRotation(glm::vec3(0.f));

            // Light volume transformation for vertex shader
            pointLightPassShader->setUniform(modelViewProjectionMatrixUniformName,
                                             m_transformer.getModelViewProjectionMatrix());

            // Set point light parameters for fragment shader
            pointLightPassShader->setUniform(lightPositionUniformName, position);
            pointLightPassShader->setUniform(lightRadiusUniformName, radius);
            pointLightPassShader->setUniform(lightColorUniformName, color);
            pointLightPassShader->setUniform(lightIntensityUniformName, intensity);
            Renderer::draw(pointLightMesh);
        }
    }

    return;
}

void DeferredRenderer::directionalLightPass(const IScene& scene, const ICamera& camera,
                                             const IWindow& window,
                                             const IGraphicsResourceManager& manager,
                                             ISceneQuery& query)
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
                shadowMapPass(scene, shadowCamera, window, manager);
            }

            // Prepare light pass frame buffer
            m_lightPassFrameBuffer.setActive(GL_FRAMEBUFFER);
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
            m_depthTexture->setActive(lightPassDepthTextureUnit);
            directionalLightPassShader->setUniform(depthTextureUniformName,
                                                   lightPassDepthTextureUnit);

            // Set texture with world space normal and specular power
            m_normalSpecularTexture->setActive(lightPassNormalSpecularTextureUnit);
            directionalLightPassShader->setUniform(normalSpecularTextureUniformName,
                                                   lightPassNormalSpecularTextureUnit);

            // Set shadow texture for shadow mapping
            m_shadowDepthTexture->setActive(lightPassShadowMapTextureUnit);
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

void DeferredRenderer::illuminationPass(const IScene& scene, const ICamera& camera,
                                         const IWindow& window,
                                         const IGraphicsResourceManager& manager,
                                         ISceneQuery& query)
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

    // Get screen space quad
    Mesh* quadMesh = manager.getMesh(m_illuminationPassScreenQuadId);
    if (quadMesh == nullptr)
    {
        spdlog::error("Mesh object for illumination pass could not be retrieved.");
        return;
    }

    // Set textures
    // L-buffer light texture
    m_lightPassTexture->setActive(illuminationPassLightTextureUnit);
    illuminationShader->setUniform(lightTextureUniformName, illuminationPassLightTextureUnit);

    // G-buffer diffuse glow texture
    m_diffuseGlowTexture->setActive(illuminationPassDiffuseGlowTextureUnit);
    illuminationShader->setUniform(diffuseGlowTextureUniformName,
                                   illuminationPassDiffuseGlowTextureUnit);

    // G-buffer depth texture
    m_depthTexture->setActive(illuminationPassDepthTextureUnit);
    illuminationShader->setUniform(depthTextureUniformName, illuminationPassDepthTextureUnit);

    // Screen parameters
    illuminationShader->setUniform(screenWidthUniformName, (float)window.getWidth());
    illuminationShader->setUniform(screenHeightUniformName, (float)window.getHeight());

    // Draw into frame buffer
    m_illumationPassFrameBuffer.setActive(GL_FRAMEBUFFER);
    Renderer::draw(quadMesh);
    m_illumationPassFrameBuffer.setInactive(GL_FRAMEBUFFER);
}

bool DeferredRenderer::initPointLightPass(IResourceManager* manager)
{
    // Point light shader
    std::string pointLightPassShaderFile("data/shader/deferred/point_light_pass.ini");
    m_pointLightPassShaderId = manager->loadShader(pointLightPassShaderFile);

    // Check if ok
    if (m_pointLightPassShaderId == invalidResource)
    {
        spdlog::error("Failed to initialize the shader from file {}.",
                  pointLightPassShaderFile.c_str());
        return false;
    }

    // Load sphere mesh for point light representation
    std::string sphereMesh = "data/mesh/sphere.obj";
    m_pointLightSphereId = manager->loadMesh(sphereMesh);
    if (m_pointLightSphereId == invalidResource)
    {
        spdlog::error("Failed to load point light volume mesh {}.", sphereMesh.c_str());
        return false;
    }

    // Attach texture for light data
    m_lightPassTexture = std::make_shared<Texture>();
    if (!m_lightPassTexture->init(800, 600, GL_RGBA16F))
    {
        spdlog::error("Failed to create color texture for light pass frame buffer.");
        return false;
    }
    m_lightPassFrameBuffer.attach(m_lightPassTexture, GL_COLOR_ATTACHMENT0);

    // Attach depth buffer
    // Depth gets discarded, depth values from geometry pass are used
    std::shared_ptr<RenderBuffer> depthBuffer = std::make_shared<RenderBuffer>();
    if (!depthBuffer->init(800, 600, GL_DEPTH_COMPONENT))
    {
        spdlog::error("Failed to create depth buffer for light pass frame buffer.");
        return false;
    }
    m_lightPassFrameBuffer.attach(depthBuffer, GL_DEPTH_ATTACHMENT);
    return true;
}

bool DeferredRenderer::initDirectionalLightPass(IResourceManager* manager)
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

    return true;
}

bool DeferredRenderer::initIlluminationPass(IResourceManager* manager)
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


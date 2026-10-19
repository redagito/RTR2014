#include <spdlog/spdlog.h>
#include "graphics/renderer/PointLightPass.h"

#include <string>

#include <glm/ext/matrix_clip_space.hpp>

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

PointLightPass::PointLightPass()
{
    return;
}

PointLightPass::~PointLightPass()
{
    return;
}

void PointLightPass::render(const IScene& scene, const ICamera& camera, const IWindow& window,
                            const IGraphicsResourceManager& manager, ISceneQuery& query,
                            FrameBuffer& lightBuffer, const std::shared_ptr<Texture>& depthTexture,
                            const std::shared_ptr<Texture>& normalSpecularTexture)
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

    // Set view and projection matrices
    m_transformer.setViewMatrix(camera.getView());
    m_transformer.setProjectionMatrix(camera.getProjection());

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
                m_shadowCubePass.render(scene, shadowCamera, window, manager);
            }

            // Prepare light pass frame buffer
            glViewport(0, 0, window.getWidth(), window.getHeight());
            lightBuffer.setActive(GL_FRAMEBUFFER);

            // No depth testing for light volumes
            glDisable(GL_DEPTH_TEST);
            // Additive blending for light accumulation
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);

            // Cull front facing faces
            glCullFace(GL_FRONT);

            // Set textures for point light pass
            // Set depth texture
            depthTexture->setActive(lightPassDepthTextureUnit);
            pointLightPassShader->setUniform(depthTextureUniformName, lightPassDepthTextureUnit);

            // Set texture with world space normal and specular power
            normalSpecularTexture->setActive(lightPassNormalSpecularTextureUnit);
            pointLightPassShader->setUniform(normalSpecularTextureUniformName,
                                             lightPassNormalSpecularTextureUnit);

            // Set shadow texture for shadow mapping
            glActiveTexture(GL_TEXTURE0 + lightPassShadowMapTextureUnit);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowCubePass.getShadowMap()->getId());
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

bool PointLightPass::init(IResourceManager* manager)
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
        spdlog::error("Failed to initialize point light volume mesh {}.", sphereMesh.c_str());
        return false;
    }

    if (!m_shadowCubePass.init(manager))
    {
        spdlog::error("Failed to initialize shadow cube pass.");
        return false;
    }

    return true;
}
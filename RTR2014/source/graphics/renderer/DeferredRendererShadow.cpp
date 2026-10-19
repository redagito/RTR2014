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


void DeferredRenderer::shadowMapPass(const IScene& scene, const ICamera& camera,
                                      const IWindow& window,
                                      const IGraphicsResourceManager& manager)
{
    m_shadowMapPassShader = manager.getShaderProgram(m_shadowMapPassShaderId);

    // Set framebuffer
    m_shadowMapBuffer.setActive(GL_FRAMEBUFFER);

    // Clear
    glClear(GL_DEPTH_BUFFER_BIT);

    // Depth
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Backface culling disabled for debugging
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Winding order, standard is counter-clockwise
    glFrontFace(GL_CCW);

    // Reset viewport
    glViewport(0, 0, 4000, 4000);
    m_shadowMapBuffer.resize(4000, 4000);

    // Stores active transformations
    Transformer transformer;

    // Set view and projection matrices
    transformer.setViewMatrix(camera.getView());
    transformer.setProjectionMatrix(camera.getProjection());

    // Query visible scene objects
    std::unique_ptr<ISceneQuery> query(std::move(scene.createQuery(camera)));

    // Send view/projection to default shader
    m_shadowMapPassShader->setUniform(viewMatrixUniformName, transformer.getViewMatrix());
    m_shadowMapPassShader->setUniform(projectionMatrixUniformName,
                                      transformer.getProjectionMatrix());

    // Traverse visible objects
    while (query->hasNextObject())
    {
        // Get next visible object
        SceneObjectId id = query->getNextObject();

        // Object attributes
        ResourceId meshId = -1;
        ResourceId materialId = -1;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        // Retrieve object data
        if (!scene.getObject(id, meshId, materialId, position, rotation, scale))
        {
            // Invalid id
            spdlog::error("Invalid scene object id {}.", id);
        }
        else
        {
            // Resolve ids
            Mesh* mesh = manager.getMesh(meshId);
            Material* material = manager.getMaterial(materialId);

            // Set transformations
            transformer.setPosition(position);
            transformer.setRotation(rotation);
            transformer.setScale(scale);

            if (material->hasCustomShader())
            {
                // Custom shaders not supported
                spdlog::warn("Deferred renderer does not support custom material shaders.");
            }
            else
            {
                // Forward draw call
                draw(mesh, transformer.getTranslationMatrix(), transformer.getRotationMatrix(),
                     transformer.getScaleMatrix(), material, manager, m_shadowMapPassShader);
            }
        }
    }

    // Post draw error check

    // Disable geometry buffer
    m_shadowMapBuffer.setInactive(GL_FRAMEBUFFER);

    glCullFace(GL_BACK);
}

struct CameraDirection
{
    GLenum cubemapFace;
    glm::vec3 target;
    glm::vec3 up;
};

CameraDirection g_cameraDirections[6] = {
    {GL_TEXTURE_CUBE_MAP_POSITIVE_X, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
    {GL_TEXTURE_CUBE_MAP_NEGATIVE_X, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
    {GL_TEXTURE_CUBE_MAP_POSITIVE_Y, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
    {GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
    {GL_TEXTURE_CUBE_MAP_POSITIVE_Z, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
    {GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)}};

void DeferredRenderer::shadowCubePass(const IScene& scene, const ICamera& camera,
                                       const IWindow& window,
                                       const IGraphicsResourceManager& manager)
{
    m_shadowCubePassShader = manager.getShaderProgram(m_shadowCubePassShaderId);
    m_shadowCubePassShader->setActive();

    // Depth
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Backface culling disabled for debugging
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glDisable(GL_BLEND);

    // Winding order, standard is counter-clockwise
    glFrontFace(GL_CCW);

    // Reset viewport
    glViewport(0, 0, 1024, 1024);
    // m_shadowMapBuffer.resize(1024, 1024);

    m_shadowCubePassShader->setUniform(lightPositionUniformName, camera.getPosition());

    glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);

    for (unsigned int i = 0; i < 6; ++i)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_shadowCubeBuffer.getId());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               g_cameraDirections[i].cubemapFace, m_shadowCubeTexture->getId(), 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glViewport(0, 0, 1024, 1024);

        // Send view/projection to default shader
        glm::mat4 view =
            glm::lookAt(camera.getPosition(), camera.getPosition() + g_cameraDirections[i].target,
                        g_cameraDirections[i].up);

        m_shadowCubePassShader->setUniform(projectionMatrixUniformName, camera.getProjection());
        m_shadowCubePassShader->setUniform(viewMatrixUniformName, view);

        // Query visible scene objects
        std::unique_ptr<ISceneQuery> query(std::move(scene.createQuery(camera)));

        // Traverse visible objects
        while (query->hasNextObject())
        {
            // Get next visible object
            SceneObjectId id = query->getNextObject();

            // Object attributes
            ResourceId meshId = -1;
            ResourceId materialId = -1;
            glm::vec3 position;
            glm::vec3 rotation;
            glm::vec3 scale;

            // Retrieve object data
            if (!scene.getObject(id, meshId, materialId, position, rotation, scale))
            {
                // Invalid id
                spdlog::error("Invalid scene object id {}.", id);
            }
            else
            {
                // Resolve ids
                Mesh* mesh = manager.getMesh(meshId);
                Material* material = manager.getMaterial(materialId);

                Transformer transformer;

                // Set transformations
                transformer.setPosition(position);
                transformer.setRotation(rotation);
                transformer.setScale(scale);

                // Forward draw call
                draw(mesh, transformer.getTranslationMatrix(), transformer.getRotationMatrix(),
                     transformer.getScaleMatrix(), material, manager, m_shadowCubePassShader);
            }
        }
    }

    // Post draw error check

    // Disable buffer
    m_shadowCubeBuffer.setInactive(GL_FRAMEBUFFER);

    glClearColor(0, 0, 0, 0);
    glCullFace(GL_BACK);
}

bool DeferredRenderer::initShadowCubePass(IResourceManager* manager)
{
    std::string shaderFile("data/shader/shadow_cube_pass.ini");
    m_shadowCubePassShaderId = manager->loadShader(shaderFile);

    if (m_shadowCubePassShaderId == invalidResource)
    {
        spdlog::error("Failed to initialize the shader from file {}.", shaderFile.c_str());
        return false;
    }

    m_shadowCubeBuffer.resize(1024, 1024);

    // Depth texture
    m_shadowCubeDepthTexture = std::make_shared<Texture>();
    m_shadowCubeDepthTexture->init(1024, 1024, GL_DEPTH_COMPONENT24);

    glBindTexture(GL_TEXTURE_2D, m_shadowCubeDepthTexture->getId());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_R32F, 1024, 1024, 0, GL_RED,
                     GL_FLOAT, NULL);
    }

    m_shadowCubeTexture = std::make_shared<Texture>(textureId, false, 1024, 1024, GL_R32F,
                                                     GL_RED);

    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowCubeBuffer.getId());
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           m_shadowCubeDepthTexture->getId(), 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    m_shadowCubeBuffer.setInactive(GL_FRAMEBUFFER);


    spdlog::info("Shadow cube buffer state: {}.", m_shadowCubeBuffer.getState().c_str());

    // Reset framebuffer
    m_shadowCubeBuffer.setInactive(GL_FRAMEBUFFER);

    return true;
}

bool DeferredRenderer::initShadowMapPass(IResourceManager* manager)
{
    std::string shaderFile("data/shader/shadow_map_pass.ini");
    m_shadowMapPassShaderId = manager->loadShader(shaderFile);

    if (m_shadowMapPassShaderId == invalidResource)
    {
        spdlog::error("Failed to initialize the shader from file {}.", shaderFile.c_str());
        return false;
    }

    // Depth texture
    m_shadowDepthTexture = std::make_shared<Texture>();
    m_shadowDepthTexture->init(4000, 4000, GL_DEPTH_COMPONENT24);

    glBindTexture(GL_TEXTURE_2D, m_shadowDepthTexture->getId());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, 0);


    // Total 96 bit per pixel
    m_shadowMapBuffer.attach(m_shadowDepthTexture, GL_DEPTH_ATTACHMENT);
    m_shadowMapBuffer.setActive(GL_FRAMEBUFFER);
    glDrawBuffer(GL_NONE);
    m_shadowMapBuffer.setInactive(GL_FRAMEBUFFER);


    spdlog::info("Shadow map buffer state: {}.", m_shadowMapBuffer.getState().c_str());

    // Reset framebuffer
    m_shadowMapBuffer.setInactive(GL_FRAMEBUFFER);

    return true;
}


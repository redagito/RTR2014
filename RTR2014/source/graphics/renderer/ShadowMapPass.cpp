#include "graphics/renderer/ShadowMapPass.h"

#include <spdlog/spdlog.h>

#include <string>

#include "graphics/ICamera.h"
#include "graphics/IGraphicsResourceManager.h"
#include "graphics/IScene.h"
#include "graphics/ISceneQuery.h"
#include "graphics/IWindow.h"
#include "graphics/Material.h"
#include "graphics/Mesh.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Texture.h"
#include "graphics/renderer/Renderer.h"
#include "graphics/renderer/Transformer.h"
#include "graphics/renderer/core/RendererCoreConfig.h"
#include "resource/IResourceManager.h"

ShadowMapPass::ShadowMapPass() { return; }

ShadowMapPass::~ShadowMapPass() { return; }

void ShadowMapPass::render(const IScene& scene, const ICamera& camera, const IWindow& window,
                           const IGraphicsResourceManager& manager)
{
    m_shadowMapPassShader = manager.getShaderProgram(m_shadowMapPassShaderId);
    if (m_shadowMapPassShader == nullptr)
    {
        spdlog::error("Shader program for shadow map pass could not be retrieved.");
        return;
    }

    // Set framebuffer
    m_shadowMapBuffer.setActive(GL_FRAMEBUFFER);

    // Reset viewport
    glViewport(0, 0, 4000, 4000);
    // The size is set before the clear, resizing reallocates the depth
    // storage, which is cleared to zero instead of one.
    m_shadowMapBuffer.resize(4000, 4000);

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

    // Stores active transformations
    Transformer transformer;

    // Set view and projection matrices
    transformer.setViewMatrix(camera.getView());
    transformer.setProjectionMatrix(camera.getProjection());

    // Query visible scene objects
    auto query = scene.createQuery(camera.getProjection() * camera.getView());

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
        bool castsShadow = true;

        // Retrieve object data
        if (!scene.getObject(id, meshId, materialId, position, rotation, scale, castsShadow))
        {
            // Invalid id
            spdlog::error("Invalid scene object id {}.", id);
        }
        else if (!castsShadow)
        {
            // The object does not cast a shadow.
            continue;
        }
        else
        {
            // Resolve ids
            Mesh* mesh = manager.getMesh(meshId);
            Material* material = manager.getMaterial(materialId);
            if (mesh == nullptr || material == nullptr)
            {
                // The graphics resource manager logged the missing resource
                continue;
            }

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
                m_shadowMapPassShader->setActive();
                Renderer::bindMaterial(
                    m_shadowMapPassShader, material, transformer.getTranslationMatrix(),
                    transformer.getRotationMatrix(), transformer.getScaleMatrix(), manager);
                Renderer::draw(mesh);
            }
        }
    }

    // Post draw error check

    // Disable geometry buffer
    m_shadowMapBuffer.setInactive(GL_FRAMEBUFFER);

    glCullFace(GL_BACK);
}

bool ShadowMapPass::init(IResourceManager* manager)
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

    glTextureParameteri(m_shadowDepthTexture->getId(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(m_shadowDepthTexture->getId(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_shadowDepthTexture->getId(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_shadowDepthTexture->getId(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // The shadow map is sampled with a comparison.
    m_shadowDepthTexture->setCompareMode(true);

    // Total 96 bit per pixel
    m_shadowMapBuffer.attach(m_shadowDepthTexture, GL_DEPTH_ATTACHMENT);
    m_shadowMapBuffer.setActive(GL_FRAMEBUFFER);
    glNamedFramebufferDrawBuffer(m_shadowMapBuffer.getId(), GL_NONE);
    m_shadowMapBuffer.setInactive(GL_FRAMEBUFFER);

    spdlog::info("Shadow map buffer state: {}.", m_shadowMapBuffer.getState().c_str());

    // Reset framebuffer
    m_shadowMapBuffer.setInactive(GL_FRAMEBUFFER);

    return true;
}

const std::shared_ptr<Texture>& ShadowMapPass::getShadowMap() const { return m_shadowDepthTexture; }
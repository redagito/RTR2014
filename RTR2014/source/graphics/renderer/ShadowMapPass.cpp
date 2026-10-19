#include <spdlog/spdlog.h>
#include "graphics/renderer/ShadowMapPass.h"

#include <string>

#include "graphics/renderer/core/RendererCoreConfig.h"

#include "resource/IResourceManager.h"

#include "graphics/IScene.h"
#include "graphics/ISceneQuery.h"
#include "graphics/ICamera.h"
#include "graphics/IWindow.h"
#include "graphics/IGraphicsResourceManager.h"
#include "graphics/Material.h"
#include "graphics/Mesh.h"
#include "graphics/Texture.h"
#include "graphics/ShaderProgram.h"
#include "graphics/renderer/Renderer.h"
#include "graphics/renderer/Transformer.h"

ShadowMapPass::ShadowMapPass()
{
    return;
}

ShadowMapPass::~ShadowMapPass()
{
    return;
}

void ShadowMapPass::render(const IScene& scene, const ICamera& camera, const IWindow& window,
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
                m_shadowMapPassShader->setActive();
                Renderer::bindMaterial(m_shadowMapPassShader, material,
                                       transformer.getTranslationMatrix(),
                                       transformer.getRotationMatrix(),
                                       transformer.getScaleMatrix(), manager);
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

const std::shared_ptr<Texture>& ShadowMapPass::getShadowMap() const
{
    return m_shadowDepthTexture;
}
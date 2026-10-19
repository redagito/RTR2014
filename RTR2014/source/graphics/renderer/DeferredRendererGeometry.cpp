#include <spdlog/spdlog.h>
#include "graphics/renderer/DeferredRenderer.h"

#include <string>

#include "graphics/renderer/core/RendererCoreConfig.h"

#include "resource/IResourceManager.h"

#include <glm/ext/matrix_transform.hpp>
#include "graphics/IScene.h"
#include "graphics/ISceneQuery.h"
#include "graphics/ICamera.h"
#include "graphics/IWindow.h"
#include "graphics/IGraphicsResourceManager.h"
#include "graphics/Material.h"
#include "graphics/Mesh.h"
#include "graphics/ShaderProgram.h"
#include "graphics/renderer/Transformer.h"


void DeferredRenderer::geometryPass(const IScene& scene, const ICamera& camera,
                                     const IWindow& window, const IGraphicsResourceManager& manager,
                                     ISceneQuery& query)
{
    // Reset viewport
    glViewport(0, 0, window.getWidth(), window.getHeight());
    m_geometryBuffer.resize(window.getWidth(), window.getHeight());
    // Set framebuffer
    m_geometryBuffer.setActive(GL_FRAMEBUFFER);
    // Clear buffer
    // TODO Should be retrieved as scene parameters
    // Diffuse, glow
    GLfloat diffuseGlow[] = {0.f, 0.f, 0.f, 0.f};
    GLfloat normalSpecular[] = {0.5f, 0.5f, 1.f, 0.f};
    glClearBufferfv(GL_COLOR, 0, diffuseGlow);
    // Normal, specular
    glClearBufferfv(GL_COLOR, 1, normalSpecular);

    // Clear
    glClear(GL_DEPTH_BUFFER_BIT);

    // Geometry pass, uses gbuffer fbo
    ShaderProgram* geometryPassShader = manager.getShaderProgram(m_geometryPassShaderId);

    // Depth
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Backface culling disabled for debugging
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Winding order, standard is counter-clockwise
    glFrontFace(GL_CCW);


    // Set view and projection matrices
    m_transformer.setViewMatrix(camera.getView());
    m_transformer.setProjectionMatrix(camera.getProjection());

    // Send view/projection to default shader
    geometryPassShader->setUniform(viewMatrixUniformName, m_transformer.getViewMatrix());
    geometryPassShader->setUniform(projectionMatrixUniformName,
                                   m_transformer.getProjectionMatrix());

    // Traverse visible objects
    while (query.hasNextObject())
    {
        // Get next visible object
        SceneObjectId id = query.getNextObject();

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
            m_transformer.setPosition(position);
            m_transformer.setRotation(rotation);
            m_transformer.setScale(scale);

            if (material->hasCustomShader())
            {
                // Custom shaders not supported
                spdlog::warn("Deferred renderer does not support custom material shaders.");
            }
            else
            {
                // Forward draw call
                draw(mesh, m_transformer.getTranslationMatrix(), m_transformer.getRotationMatrix(),
                     m_transformer.getScaleMatrix(), material, manager, geometryPassShader);
            }
        }
    }

    // Post draw error check

    // Disable geometry buffer
    m_geometryBuffer.setInactive(GL_FRAMEBUFFER);
}

void DeferredRenderer::draw(Mesh* mesh, const glm::mat4& translation, const glm::mat4& rotation,
                             const glm::mat4& scale, Material* material,
                             const IGraphicsResourceManager& manager, ShaderProgram* shader)
{

    shader->setActive();

    bindMaterial(shader, material, translation, rotation, scale, manager);

    // Draw mesh
    // TODO Consider custom shader bindings for meshes
    Renderer::draw(mesh);

    // TODO Cleanup?
}

bool DeferredRenderer::initGeometryPass(IResourceManager* manager)
{
    // Init geometry pass shader
    // TODO Read file name from config?
    // Geometry pass shader for filling gbuffer
    std::string geometryPassShaderFile("data/shader/deferred/geometry_pass.ini");
    m_geometryPassShaderId = manager->loadShader(geometryPassShaderFile);

    // Check if ok
    if (m_geometryPassShaderId == invalidResource)
    {
        spdlog::error("Failed to initialize the shader from file {}.", geometryPassShaderFile.c_str());
        return false;
    }

    // Init gbuffer
    // Diffuse texture, stores base color and glow mask.
    m_diffuseGlowTexture = std::make_shared<Texture>();
    if (!m_diffuseGlowTexture->init(800, 600, GL_RGBA8))
    {
        spdlog::error("Failed to initialize the diffuse and glow texture.");
        return false;
    }

    // Normal texture, store x and y, z normals and specularity.
    m_normalSpecularTexture = std::make_shared<Texture>();
    if (!m_normalSpecularTexture->init(800, 600, GL_RGBA16F))
    {
        spdlog::error("Failed to initialize the normal and specular texture.");
        return false;
    }

    // Depth texture
    m_depthTexture = std::make_shared<Texture>();
    if (!m_depthTexture->init(800, 600, GL_DEPTH_COMPONENT24))
    {
        spdlog::error("Failed to initialize the depth texture.");
        return false;
    }


    // Total 96 bit per pixel
    m_geometryBuffer.attach(m_depthTexture, GL_DEPTH_ATTACHMENT);
    m_geometryBuffer.attach(m_diffuseGlowTexture, GL_COLOR_ATTACHMENT0);
    m_geometryBuffer.attach(m_normalSpecularTexture, GL_COLOR_ATTACHMENT1);


    spdlog::info("GBuffer state: {}.", m_geometryBuffer.getState().c_str());

    // Reset framebuffer
    m_geometryBuffer.setInactive(GL_FRAMEBUFFER);

    return true;
}


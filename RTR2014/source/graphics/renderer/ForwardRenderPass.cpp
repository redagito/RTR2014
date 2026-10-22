#include "graphics/renderer/ForwardRenderPass.h"

#include <string>

#include "graphics/ICamera.h"
#include "graphics/IGraphicsResourceManager.h"
#include "graphics/IScene.h"
#include "graphics/ISceneQuery.h"
#include "graphics/IWindow.h"
#include "graphics/Material.h"
#include "graphics/Mesh.h"
#include "graphics/ShaderProgram.h"
#include "graphics/renderer/Renderer.h"
#include "graphics/renderer/core/RendererCoreConfig.h"
#include "resource/IResourceManager.h"
#include "util/FatalError.h"

ForwardRenderPass::ForwardRenderPass() { return; }

ForwardRenderPass::~ForwardRenderPass() { return; }

void ForwardRenderPass::render(const IScene& scene, const ICamera& camera, const IWindow& window,
                               const IGraphicsResourceManager& manager)
{
    // The sky fills the background, it is drawn before the scene.
    m_skyPass.render(window, manager, camera);

    // Set forward shader as active
    m_currentShader = manager.getShaderProgram(m_forwardShader);
    if (m_currentShader == nullptr)
    {
        fatalError("Shader program for forward render pass could not be retrieved.");
    }

    // Set view and projection matrices
    m_currentView = camera.getView();
    m_currentProjection = camera.getProjection();

    // Query visible scene objects
    auto query = scene.createQuery(camera.getProjection() * camera.getView());

    // Send view/projection to default shader
    m_currentShader->setUniform(viewMatrixUniformName, m_currentView);
    m_currentShader->setUniform(projectionMatrixUniformName, m_currentProjection);

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
            fatalError("Invalid scene object id {}.", id);
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
            m_transformer.setPosition(position);
            m_transformer.setRotation(rotation);
            m_transformer.setScale(scale);

            // Forward draw call
            draw(mesh, m_transformer.getTranslationMatrix(), m_transformer.getRotationMatrix(),
                 m_transformer.getScaleMatrix(), material, manager);
        }
    }

    // Post draw error check
}

void ForwardRenderPass::draw(Mesh* mesh, const glm::mat4& translation, const glm::mat4& rotation,
                             const glm::mat4& scale, Material* material,
                             const IGraphicsResourceManager& manager)
{
    // Decide which shader program to use
    ShaderProgram* shader = m_currentShader;
    if (material->hasCustomShader())
    {
        shader = material->getCustomShader();
    }
    shader->setActive();

    // Custom shader needs access to view/projection matrices
    if (material->hasCustomShader())
    {
        shader->setUniform(viewMatrixUniformName, m_currentView);
        shader->setUniform(projectionMatrixUniformName, m_currentProjection);
    }

    // The model matrix holds the translation, the rotation and the scale of
    // the object, the forward shader does not need them separately.
    shader->setUniform(modelMatrixUniformName, translation * rotation * scale);
    Renderer::bindTexture(shader, diffuseTextureUniformName, diffuseTextureUnit,
                          material->getDiffuse(), manager.getDefaultDiffuseTexture());
    Renderer::bindTexture(shader, specularTextureUniformName, specularTextureUnit,
                          material->getSpecular(), manager.getDefaultSpecularTexture());
    Renderer::bindTexture(shader, glowTextureUniformName, glowTextureUnit, material->getGlow(),
                          manager.getDefaultGlowTexture());
    // TODO Sort materials with alpha texture for blending.
    // The forward shader uses neither the normal map nor the alpha texture.

    // Draw mesh
    // TODO Consider custom shader bindings for meshes
    Renderer::draw(mesh);

    // TODO Cleanup?
}

bool ForwardRenderPass::init(IResourceManager* manager, const std::string& skyCubemapFile)
{
    // TODO Read file name from config?
    std::string defaultShaderFile("data/shader/forward_test_0.ini");
    m_forwardShader = manager->loadShader(defaultShaderFile);

    // Check if ok
    if (m_forwardShader == invalidResource)
    {
        fatalError("Failed to initialize the default shader from file {}.",
                   defaultShaderFile.c_str());
    }

    // The sky that fills the background of the scene
    if (!m_skyPass.init(manager, skyCubemapFile))
    {
        fatalError("Failed to initialize the sky.");
    }
    return true;
}
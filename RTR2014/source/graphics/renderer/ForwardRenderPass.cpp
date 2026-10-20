#include <spdlog/spdlog.h>
#include "graphics/renderer/ForwardRenderPass.h"

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
#include "graphics/ShaderProgram.h"

#include "graphics/renderer/Renderer.h"

ForwardRenderPass::ForwardRenderPass()
{
    return;
}

ForwardRenderPass::~ForwardRenderPass()
{
    return;
}

void ForwardRenderPass::render(const IScene& scene, const ICamera& camera, const IWindow& window,
                               const IGraphicsResourceManager& manager)
{
    // Set forward shader as active
    m_currentShader = manager.getShaderProgram(m_forwardShader);
    if (m_currentShader == nullptr)
    {
        spdlog::error("Shader program for forward render pass could not be retrieved.");
        return;
    }

    // Set view and projection matrices
    m_currentView = camera.getView();
    m_currentProjection = camera.getProjection();

    // Query visible scene objects
    std::unique_ptr<ISceneQuery> query(
        std::move(scene.createQuery(camera.getProjection() * camera.getView())));

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

    Renderer::bindMaterial(shader, material, translation, rotation, scale, manager);

    // Draw mesh
    // TODO Consider custom shader bindings for meshes
    Renderer::draw(mesh);

    // TODO Cleanup?
}

bool ForwardRenderPass::init(IResourceManager* manager)
{
    // TODO Read file name from config?
    std::string defaultShaderFile("data/shader/forward_test_0.ini");
	m_forwardShader = manager->loadShader(defaultShaderFile);

    // Check if ok
	if (m_forwardShader == invalidResource)
    {
        spdlog::error("Failed to initialize the default shader from file {}.",
                  defaultShaderFile.c_str());
        return false;
    }
    return true;
}
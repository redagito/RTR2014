#include "CRenderer.h"

#include <cassert>

#include <glm/ext.hpp>

#include "graphics/IScene.h"
#include "graphics/ICamera.h"
#include "graphics/IWindow.h"

CRenderer::CRenderer(const std::shared_ptr<IResourceManager>& resourceManager)
    : m_resourceManager(resourceManager)
{
    // Add resource listener
    m_resourceManager->addResourceListener(this);
    return;
}

CRenderer::~CRenderer()
{
    // Remove resource listener
    m_resourceManager->removeResourceListener(this);
}

void CRenderer::draw(const IScene& scene, const ICamera& camera, const IWindow& window)
{
    // Draw init
    window.setActive();

    // TODO Implement multi pass system

    // Query visible objects
    IScene::ObjectQueryId objectQuery = scene.queryVisibleObjects(camera);

    // Traverse visible objects
    IScene::ObjectId sceneObject = scene.getNextObject(objectQuery);
    while (sceneObject > -1)
    {
        // Get object attributes
        IResourceManager::ResourceId mesh = scene.getMesh(sceneObject);
        IResourceManager::ResourceId material = scene.getMaterial(sceneObject);

        // Get object transformation
        glm::vec3 position = scene.getPosition(sceneObject);
        glm::vec3 rotation = scene.getRotation(sceneObject);
        glm::vec3 scale = scene.getScale(sceneObject);

        // Draw call
        draw(mesh, position, rotation, scale, material);
    }
}

void CRenderer::draw(IResourceManager::ResourceId meshId, const glm::vec3& position,
                     const glm::vec3& rotation, const glm::vec3& scale,
                     IResourceManager::ResourceId materialId)
{
    // Resolve ids
    const std::unique_ptr<CMesh>& mesh = getMesh(meshId);
    const std::unique_ptr<CMaterial>& material = getMaterial(materialId);

    // Create matrices
    glm::mat4 translationMatrix = glm::translate(position);
    // TODO Is this correct?
    glm::mat4 rotationMatrix = glm::rotate(1.f, rotation);
    glm::mat4 scaleMatrix = glm::scale(scale);

    // Draw call
    draw(mesh, translationMatrix, rotationMatrix, scaleMatrix, material);
}

void CRenderer::draw(const std::unique_ptr<CMesh>& mesh, const glm::mat4& translation,
                     const glm::mat4& rotation, const glm::mat4& scale,
                     const std::unique_ptr<CMaterial>& material)
{
    // TODO Rendering logic
}

void CRenderer::onAttach(IResourceManager* resourceManager)
{
    // TODO Implement
    return;
}

void CRenderer::onDetach(IResourceManager* resourceManager)
{
    // TODO Implement
    return;
}

void CRenderer::notify(IResourceManager::EResourceType type, IResourceManager::ResourceId,
                       IResourceManager::EListenerEvent event, IResourceManager* resourceManager)
{
    // Handle events
    switch (event)
    {
    case IResourceManager::EListenerEvent::Create:
        break;

    case IResourceManager::EListenerEvent::Change:
        break;

    case IResourceManager::EListenerEvent::Delete:
        break;
    default:
        // TODO Error handling
        break;
    }
}

const std::unique_ptr<CMesh>& CRenderer::getMesh(IResourceManager::ResourceId meshId)
{
    // Search for id
    auto iter = m_meshes.find(meshId);

    // Id must exist
    // TODO Allow mesh loading if not found?
    assert(iter != m_meshes.end());

    return iter->second;
}

const std::unique_ptr<CMaterial>& CRenderer::getMaterial(IResourceManager::ResourceId materialId)
{
    // Search for id
    auto iter = m_materials.find(materialId);

    // Id must exist
    // TODO Allow material loading if not found?
    assert(iter != m_materials.end());

    return iter->second;
}

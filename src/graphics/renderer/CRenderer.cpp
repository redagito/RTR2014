#include "CRenderer.h"

#include <cassert>

#include <glm/ext.hpp>

#include "graphics/IScene.h"
#include "graphics/ICamera.h"
#include "graphics/IWindow.h"
#include "graphics/IResourceManager.h"

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

    // Query visible scene objects
    std::unique_ptr<ISceneQuery> query(std::move(scene.createQuery(camera)));

    // Traverse visible objects
    while (query->hasNextObject())
    {
        // Get next visible object
        SceneObjectId id = query->getNextObject();

        // Get object attributes
        ResourceId mesh = scene.getMesh(id);
        ResourceId material = scene.getMaterial(id);

        // Get object transformation
        glm::vec3 position = scene.getObjectPosition(id);
        glm::vec3 rotation = scene.getObjectRotation(id);
        glm::vec3 scale = scene.getObjectScale(id);

        // Draw call
        draw(mesh, position, rotation, scale, material);
    }
}

void CRenderer::draw(ResourceId meshId, const glm::vec3& position, const glm::vec3& rotation,
                     const glm::vec3& scale, ResourceId materialId)
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

void CRenderer::notify(EResourceType type, ResourceId id, EListenerEvent event,
                       IResourceManager* resourceManager)
{
    // Handle events
    switch (type)
    {
    case EResourceType::Image:
        handleImageEvent(id, event, resourceManager);
        break;
    case EResourceType::Material:
        handleMaterialEvent(id, event, resourceManager);
        break;
    case EResourceType::Mesh:
        handleMeshEvent(id, event, resourceManager);
        break;
    case EResourceType::Shader:
        handleShaderEvent(id, event, resourceManager);
        break;
    case EResourceType::String:
        handleStringEvent(id, event, resourceManager);
        break;
    default:
        assert(false && "Unknown resource type.");
        break;
    }
}

const std::unique_ptr<CMesh>& CRenderer::getMesh(ResourceId meshId)
{
    // Search for id
    auto iter = m_meshes.find(meshId);

    // Id must exist
    // TODO Allow mesh loading if not found?
    assert(iter != m_meshes.end());

    return iter->second;
}

const std::unique_ptr<CMaterial>& CRenderer::getMaterial(ResourceId materialId)
{
    // Search for id
    auto iter = m_materials.find(materialId);

    // Id must exist
    // TODO Allow material loading if not found?
    assert(iter != m_materials.end());

    return iter->second;
}

void CRenderer::handleImageEvent(ResourceId id, EListenerEvent event,
                                 IResourceManager* resourceManager)
{
    std::vector<unsigned char> data;
    unsigned int width;
    unsigned int height;
    EColorFormat format;

    switch (event)
    {
    case EListenerEvent::Create:
        assert(m_textures.count(id) == 0 && "Texture id already exists");

        if (!resourceManager->getImage(id, data, width, height, format))
        {
            assert(false && "Failed to access image resource");
        }
        // Create new texture
        m_textures[id] =
            std::move(std::unique_ptr<CTexture>(new CTexture(data, width, height, format, 4)));
        break;

    case EListenerEvent::Change:
        assert(m_textures.count(id) == 1 && "Texture id does not exist");

        if (!resourceManager->getImage(id, data, width, height, format))
        {
            assert(false && "Failed to access image resource");
        }
        // Reinitialize texture on change
        m_textures.at(id)->init(data, width, height, format, 4);
        break;

    case EListenerEvent::Delete:
        // Keep texture?
        break;

    default:
        break;
    }
}

void CRenderer::handleMeshEvent(ResourceId id, EListenerEvent event,
                                IResourceManager* resourceManager)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<float> normals;
    std::vector<float> uvs;
    EPrimitiveType type;

    switch (event)
    {
    case EListenerEvent::Create:
        assert(m_meshes.count(id) == 0 && "Mesh id already exists");

        if (!resourceManager->getMesh(id, vertices, indices, normals, uvs, type))
        {
            assert(false && "Failed to access mesh resource");
        }
        // Create new texture
        m_meshes[id] =
            std::move(std::unique_ptr<CMesh>(new CMesh(vertices, indices, normals, uvs, type)));
        break;

    case EListenerEvent::Change:
        assert(m_meshes.count(id) == 1 && "Mesh id does not exist");

        if (!resourceManager->getMesh(id, vertices, indices, normals, uvs, type))
        {
            assert(false && "Failed to access mesh resource");
        }
        // Reinitialize mesh on change
        m_meshes.at(id)->init(vertices, indices, normals, uvs, type);
        break;

    case EListenerEvent::Delete:
        // Keep mesh?
        break;

    default:
        break;
    }
}

void CRenderer::handleMaterialEvent(ResourceId, EListenerEvent event,
                                    IResourceManager* resourceManager)
{
}

void CRenderer::handleShaderEvent(ResourceId, EListenerEvent event,
                                  IResourceManager* resourceManager)
{
}

void CRenderer::handleStringEvent(ResourceId, EListenerEvent event,
                                  IResourceManager* resourceManager)
{
}
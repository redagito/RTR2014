#include "CResourceManager.h"

#include "graphics/IResourceListener.h"

CResourceManager::CResourceManager() : m_nextMeshId(0), m_nextImageId(0), m_nextMaterialId(0)
{
    return;
}

ResourceId CResourceManager::createMesh(const std::vector<float>& vertices,
                                        const std::vector<unsigned int>& indices,
                                        const std::vector<float>& normals,
                                        const std::vector<float>& uvs, EPrimitiveType type)
{
    // Create mesh id
    ResourceId id = m_nextMeshId;
    ++m_nextMeshId;

    // Add mesh
    m_meshes[id] = SMesh(vertices, indices, normals, uvs, type);

    // Notify listener with create event
    notifyResourceListeners(EResourceType::Mesh, id, EListenerEvent::Create);
    return id;
}

ResourceId CResourceManager::createImage(const std::vector<unsigned char>& imageData,
                                         unsigned int width, unsigned int height,
                                         EColorFormat format)
{
    // Create image
    ResourceId id = m_nextImageId;
    ++m_nextImageId;

    // TODO Sanity check if mesh id exists?
    // Add mesh
    m_images[id] = SImage(imageData, width, height, format);

    // Notify listener with create event
    notifyResourceListeners(EResourceType::Image, id, EListenerEvent::Create);
    return id;
}

ResourceId CResourceManager::createMaterial(ResourceId diffuse, ResourceId alpha, ResourceId normal,
                                            ResourceId specular, ResourceId glow)
{
    // Create material
    ResourceId id = m_nextMaterialId;
    ++m_nextMaterialId;

    // Add mesh
    m_materials[id] = SMaterial(diffuse, alpha, normal, specular, glow);

    // Notify listener with create event
    notifyResourceListeners(EResourceType::Material, id, EListenerEvent::Create);
    return id;
}

ResourceId CResourceManager::createString(const std::string& text) { return -1; }

ResourceId CResourceManager::createShader(ResourceId vertex, ResourceId tessCtrl,
                                          ResourceId tessEval, ResourceId geometry,
                                          ResourceId fragment)
{
    return -1;
}

void CResourceManager::addResourceListener(IResourceListener* listener)
{
    m_resourceListeners.push_back(listener);
    listener->onAttach(this);
}

void CResourceManager::removeResourceListener(IResourceListener* listener)
{
    m_resourceListeners.remove(listener);
    listener->onDetach(this);
}

void CResourceManager::notifyResourceListeners(EResourceType type, ResourceId id,
                                               EListenerEvent event)
{
    for (const auto& listener : m_resourceListeners)
    {
        listener->notify(type, id, event, this);
    }
}

#include "CResourceManager.h"

CResourceManager::CResourceManager() : m_nextMeshId(0), m_nextImageId(0), m_nextMaterialId(0) {}

IResourceManager::MeshId CResourceManager::createMesh(const std::vector<float>& vertices,
                                                      const std::vector<unsigned int>& indices,
                                                      const std::vector<float>& normals,
                                                      const std::vector<float>& uvs,
                                                      EPrimitiveType type)
{
    // Create mesh id
    MeshId id = m_nextMeshId;
    ++m_nextMeshId;

    // Add mesh
    m_meshes[id] = SMesh(vertices, indices, normals, uvs, type);

    // Notify listener with create event
    notifyMeshListeners(id, IResourceManager::EListenerEvent::Create);
    return id;
}

void CResourceManager::addMeshListener(const IResourceManager::MeshListener& meshListener)
{
    m_meshListeners.push_back(meshListener);
}

IResourceManager::ImageId CResourceManager::createImage(const std::vector<unsigned char>& imageData,
                                                        unsigned int width, unsigned int height,
                                                        EColorFormat format)
{
    // Create image
    ImageId id = m_nextImageId;
    ++m_nextImageId;

    // Add image
    m_images[id] = SImage(imageData, width, height, format);

    // Notify listener with create event
    notifyImageListeners(id, IResourceManager::EListenerEvent::Create);
    return id;
}

void CResourceManager::addImageListener(const IResourceManager::ImageListener& imageListener)
{
    m_imageListeners.push_back(imageListener);
}

IResourceManager::MaterialId CResourceManager::createMaterial(IResourceManager::ImageId diffuse,
                                                              IResourceManager::ImageId alpha,
                                                              IResourceManager::ImageId normal,
                                                              IResourceManager::ImageId specular,
                                                              IResourceManager::ImageId glow)
{
    // Create material
    MaterialId id = m_nextMaterialId;
    ++m_nextMaterialId;

    // Add material
    m_materials[id] = SMaterial(diffuse, alpha, normal, specular, glow);

    // Notify listener with create event
    notifyMaterialListeners(id, IResourceManager::EListenerEvent::Create);
    return id;
}

void CResourceManager::addMaterialListener(
    const IResourceManager::MaterialListener& materialListener)
{
    return;
}

void CResourceManager::notifyMeshListeners(IResourceManager::MeshId id,
                                           IResourceManager::EListenerEvent event)
{
    for (auto listener : m_meshListeners)
    {
        listener(id, event);
    }
}

void CResourceManager::notifyImageListeners(IResourceManager::ImageId id,
                                            IResourceManager::EListenerEvent event)
{
    for (auto listener : m_imageListeners)
    {
        listener(id, event);
    }
}

void CResourceManager::notifyMaterialListeners(IResourceManager::MaterialId id,
                                               IResourceManager::EListenerEvent event)
{
    for (auto listener : m_materialListeners)
    {
        listener(id, event);
    }
}

CResourceManager::SMesh::SMesh() {}

CResourceManager::SMesh::SMesh(std::vector<float> vertices, std::vector<unsigned int> indices,
                               std::vector<float> normals, std::vector<float> uvs,
                               IResourceManager::EPrimitiveType type)
    : m_vertices(vertices), m_indices(indices), m_normals(normals), m_uvs(uvs), m_type(type)
{
}

CResourceManager::SImage::SImage() {}

CResourceManager::SImage::SImage(std::vector<unsigned char> data, unsigned int width,
                                 unsigned int height, IResourceManager::EColorFormat format)
    : m_data(data), m_width(width), m_height(height), m_format(format)
{
}

CResourceManager::SMaterial::SMaterial() {}

CResourceManager::SMaterial::SMaterial(IResourceManager::ImageId diffuse,
                                       IResourceManager::ImageId alpha,
                                       IResourceManager::ImageId normal,
                                       IResourceManager::ImageId specular,
                                       IResourceManager::ImageId glow)
    : m_diffuse(diffuse), m_alpha(alpha), m_normal(normal), m_specular(specular), m_glow(glow)
{
}
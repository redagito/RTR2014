#include "CResourceManager.h"

#include "graphics/IResourceListener.h"

CResourceManager::CResourceManager() : m_nextMeshId(0), m_nextImageId(0), m_nextMaterialId(0)
{
    return;
}

IResourceManager::ResourceId CResourceManager::createMesh(const std::vector<float>& vertices,
                                                          const std::vector<unsigned int>& indices,
                                                          const std::vector<float>& normals,
                                                          const std::vector<float>& uvs,
                                                          EPrimitiveType type)
{
    // Create mesh id
    ResourceId id = m_nextMeshId;
    ++m_nextMeshId;

    // Add mesh
    m_meshes[id] = SMesh(vertices, indices, normals, uvs, type);

    // Notify listener with create event
    notifyResourceListeners(IResourceManager::EResourceType::Mesh, id,
                            IResourceManager::EListenerEvent::Create);
    return id;
}

IResourceManager::ResourceId CResourceManager::createImage(
    const std::vector<unsigned char>& imageData, unsigned int width, unsigned int height,
    EColorFormat format)
{
    // Create image
    ResourceId id = m_nextImageId;
    ++m_nextImageId;

    // TODO Sanity check if mesh id exists?
    // Add mesh
    m_images[id] = SImage(imageData, width, height, format);

    // Notify listener with create event
    notifyResourceListeners(IResourceManager::EResourceType::Image, id,
                            IResourceManager::EListenerEvent::Create);
    return id;
}

IResourceManager::ResourceId CResourceManager::createMaterial(IResourceManager::ResourceId diffuse,
                                                              IResourceManager::ResourceId alpha,
                                                              IResourceManager::ResourceId normal,
                                                              IResourceManager::ResourceId specular,
                                                              IResourceManager::ResourceId glow)
{
    // Create material
    ResourceId id = m_nextMaterialId;
    ++m_nextMaterialId;

    // Add mesh
    m_materials[id] = SMaterial(diffuse, alpha, normal, specular, glow);

    // Notify listener with create event
    notifyResourceListeners(IResourceManager::EResourceType::Material, id,
                            IResourceManager::EListenerEvent::Create);
    return id;
}

IResourceManager::ResourceId CResourceManager::createString(const std::string& text) { return -1; }

IResourceManager::ResourceId CResourceManager::createShader(ResourceId vertex, ResourceId tessCtrl,
                                                            ResourceId tessEval,
                                                            ResourceId geometry,
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

void CResourceManager::notifyResourceListeners(IResourceManager::EResourceType type,
                                               IResourceManager::ResourceId id,
                                               IResourceManager::EListenerEvent event)
{
    for (const auto& listener : m_resourceListeners)
    {
        listener->notify(type, id, event, this);
    }
}

CResourceManager::SMesh::SMesh(std::vector<float> vertices, std::vector<unsigned int> indices,
                               std::vector<float> normals, std::vector<float> uvs,
                               EPrimitiveType type)
    : m_vertices(vertices), m_indices(indices), m_normals(normals), m_uvs(uvs), m_type(type)
{
    return;
}

CResourceManager::SMesh::SMesh() : m_type(IResourceManager::EPrimitiveType::Invalid) { return; }

CResourceManager::SImage::SImage(std::vector<unsigned char> data, unsigned int width,
                                 unsigned int height, EColorFormat format)
    : m_data(data), m_width(width), m_height(height), m_format(format)
{
    return;
}

CResourceManager::SImage::SImage()
    : m_width(0), m_height(0), m_format(IResourceManager::EColorFormat::Invalid)
{
    return;
}

CResourceManager::SMaterial::SMaterial(ResourceId diffuse, ResourceId alpha, ResourceId normal,
                                       ResourceId specular, ResourceId glow)
    : m_diffuse(diffuse), m_alpha(alpha), m_normal(normal), m_specular(specular), m_glow(glow)
{
    return;
}

CResourceManager::SMaterial::SMaterial()
    : m_diffuse(-1), m_alpha(-1), m_normal(-1), m_specular(-1), m_glow(-1)
{
    return;
}

CResourceManager::SShader::SShader()
    : m_vertex(-1), m_tessCtrl(-1), m_tessEval(-1), m_geometry(-1), m_fragment(-1)
{
    return;
}

CResourceManager::SShader::SShader(ResourceId vertex, ResourceId tessCtrl, ResourceId tessEval,
                                   ResourceId geometry, ResourceId fragment)
    : m_vertex(vertex),
      m_tessCtrl(tessCtrl),
      m_tessEval(tessEval),
      m_geometry(geometry),
      m_fragment(fragment)
{
    return;
}

#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <list>

#include "graphics/IResourceManager.h"

class CResourceManager : public IResourceManager
{
   public:
    CResourceManager();

    ResourceId createMesh(const std::vector<float>& vertices,
                          const std::vector<unsigned int>& indices,
                          const std::vector<float>& normals, const std::vector<float>& uvs,
                          EPrimitiveType type);
    ResourceId createImage(const std::vector<unsigned char>& imageData, unsigned int width,
                           unsigned int height, EColorFormat format);
    ResourceId createMaterial(ResourceId diffuse, ResourceId alpha, ResourceId normal,
                              ResourceId specular, ResourceId glow);
    ResourceId createString(const std::string& text);
    ResourceId createShader(ResourceId vertex, ResourceId tessCtrl, ResourceId tessEval,
                            ResourceId geometry, ResourceId fragment);

    void addResourceListener(IResourceListener* listener);
    void removeResourceListener(IResourceListener* listener);

   protected:
    void notifyResourceListeners(EResourceType type, ResourceId id, EListenerEvent event);

   private:
    /**
     * \brief Mesh data.
     */
    struct SMesh
    {
        SMesh();
        SMesh(std::vector<float> vertices, std::vector<unsigned int> indices,
              std::vector<float> normals, std::vector<float> uvs, EPrimitiveType type);
        std::vector<float> m_vertices;
        std::vector<unsigned int> m_indices;
        std::vector<float> m_normals;
        std::vector<float> m_uvs;
        EPrimitiveType m_type;
    };

    /**
     * \brief Image data.
     */
    struct SImage
    {
        SImage();
        SImage(std::vector<unsigned char> data, unsigned int width, unsigned int height,
               EColorFormat format);
        std::vector<unsigned char> m_data;
        unsigned int m_width;
        unsigned int m_height;
        EColorFormat m_format;
    };

    /**
     * \brief Material data.
     */
    struct SMaterial
    {
        SMaterial();
        SMaterial(ResourceId diffuse, ResourceId alpha, ResourceId normal, ResourceId specular,
                  ResourceId glow);
        ResourceId m_diffuse;
        ResourceId m_alpha;
        ResourceId m_normal;
        ResourceId m_specular;
        ResourceId m_glow;
    };

    struct SShader
    {
        SShader();
        SShader(ResourceId vertex, ResourceId tessCtrl, ResourceId tessEval, ResourceId geometry,
                ResourceId fragment);
        ResourceId m_vertex;
        ResourceId m_tessCtrl;
        ResourceId m_tessEval;
        ResourceId m_geometry;
        ResourceId m_fragment;
    };

    ResourceId m_nextMeshId;     /**< Next free mesh id. */
    ResourceId m_nextImageId;    /**< Next free image id. */
    ResourceId m_nextMaterialId; /**< Next free material id. */

    // TODO Change to vector
    std::unordered_map<ResourceId, SMesh> m_meshes;        /**< Loaded meshes. */
    std::unordered_map<ResourceId, SImage> m_images;       /**< Loaded images. */
    std::unordered_map<ResourceId, SMaterial> m_materials; /**< Loaded materials. */
    std::unordered_map<ResourceId, std::string> m_strings; /**< Loaded strings. */
    std::unordered_map<ResourceId, SShader> m_shaders;     /**< Loaded shaders. */

    std::list<IResourceListener*> m_resourceListeners; /**< Registered listeners. */
};

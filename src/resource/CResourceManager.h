#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <list>

#include "IResourceManager.h"

class CResourceManager : public IResourceManager
{
   public:
    CResourceManager();

    MeshId createMesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices,
                      const std::vector<float>& normals, const std::vector<float>& uvs,
                      EPrimitiveType type);
    void addMeshListener(const MeshListener& meshListener);

    ImageId createImage(const std::vector<unsigned char>& imageData, unsigned int width,
                        unsigned int height, EColorFormat format);
    void addImageListener(const ImageListener& imageListener);

    MaterialId createMaterial(ImageId diffuse, ImageId alpha, ImageId normal, ImageId specular,
                              ImageId glow);
    void addMaterialListener(const MaterialListener& materialListener);

   protected:
    void notifyMeshListeners(MeshId id, EListenerEvent event);
    void notifyImageListeners(ImageId id, EListenerEvent event);
    void notifyMaterialListeners(MaterialId id, EListenerEvent event);

   private:
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

    struct SMaterial
    {
        SMaterial();
        SMaterial(ImageId diffuse, ImageId alpha, ImageId normal, ImageId specular, ImageId glow);

        ImageId m_diffuse;
        ImageId m_alpha;
        ImageId m_normal;
        ImageId m_specular;
        ImageId m_glow;
    };

    MeshId m_nextMeshId;
    ImageId m_nextImageId;
    MaterialId m_nextMaterialId;

    // TODO Change to vector
    std::unordered_map<MeshId, SMesh> m_meshes;
    std::unordered_map<ImageId, SImage> m_images;
    std::unordered_map<MaterialId, SMaterial> m_materials;

    std::list<MeshListener> m_meshListeners;
    std::list<ImageListener> m_imageListeners;
    std::list<MaterialListener> m_materialListeners;
};

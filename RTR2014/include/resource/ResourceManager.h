#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <list>
#include <string>

#include "resource/IResourceManager.h"

#include "Image.h"
#include "MaterialData.h"
#include "MeshData.h"
#include "Shader.h"

/**
* \brief Resource manager implementation.
*/
class ResourceManager : public IResourceManager
{
   public:
    ResourceManager();

    ResourceId createMesh(std::vector<float> vertices, std::vector<unsigned int> indices,
                          std::vector<float> normals, std::vector<float> uvs, PrimitiveType type);

    ResourceId loadMesh(const std::string& file) override;

    bool getMesh(ResourceId id, std::vector<float>& vertices, std::vector<unsigned int>& indices,
                 std::vector<float>& normals, std::vector<float>& uvs, PrimitiveType& type) const;

    bool getMeshBounds(ResourceId id, OrientedBoundingBox& bounds) const override;

    ResourceId createImage(std::vector<unsigned char> imageData, unsigned int width,
                           unsigned int height, ColorFormat format);

    ResourceId loadImage(const std::string& file, ColorFormat format) override;

    bool getImage(ResourceId id, std::vector<unsigned char>& data, unsigned int& width,
                  unsigned int& height, ColorFormat& format) const;

    ResourceId createMaterial(ResourceId diffuse, ResourceId normal, ResourceId specular,
                              ResourceId glow, ResourceId alpha, ResourceId customShader);

    ResourceId loadMaterial(const std::string& file) override;

    bool getMaterial(ResourceId id, ResourceId& diffuse, ResourceId& normal, ResourceId& specular,
                     ResourceId& glow, ResourceId& alpha, ResourceId& customShader) const;

    ResourceId createString(const std::string& text);

    ResourceId loadString(const std::string& file);

    bool getString(ResourceId id, std::string& text) const;

    ResourceId createShader(ResourceId vertex, ResourceId tessCtrl, ResourceId tessEval,
                            ResourceId geometry, ResourceId fragment);

    ResourceId loadShader(const std::string& file) override;

    bool getShader(ResourceId id, ResourceId& vertex, ResourceId& tessCtrl, ResourceId& tessEval,
                   ResourceId& geometry, ResourceId& fragment) const;

    void addResourceListener(IResourceListener* listener);
    void removeResourceListener(IResourceListener* listener);

    /**
    * \brief Loads meshes and the images of the given materials in parallel.
    *
    * The files are parsed and decoded on worker threads, but not published
    * yet. Publishing happens when the resource is loaded for the first time,
    * which must occur on the thread owning the graphics context.
    */
    void preload(const std::vector<std::string>& meshFiles,
                 const std::vector<std::string>& materialFiles);

    void notifyResourceListeners(ResourceType type, ResourceId id, ListenerEvent event);

   private:
    /**
    * \brief Parses a mesh file without publishing it.
    */
    static bool parseMesh(const std::string& file, MeshData& data);

    /**
    * \brief Decodes an image file without publishing it.
    */
    static bool decodeImage(const std::string& file, ColorFormat format, Image& image);

    /**
    * \brief Returns the image files referenced by a material file.
    */
    static bool getMaterialImages(const std::string& file,
                                  std::vector<std::pair<std::string, ColorFormat>>& images);

    ResourceId m_nextMeshId;     /**< Next free mesh id. */
    ResourceId m_nextImageId;    /**< Next free image id. */
    ResourceId m_nextMaterialId; /**< Next free material id. */
    ResourceId m_nextStringId;   /**< Next free string id. */
    ResourceId m_nextShaderId;   /**< Next free shader id. */

    // TODO Change to vector?
    std::unordered_map<ResourceId, MeshData> m_meshes;        /**< Loaded meshes. */
    std::unordered_map<ResourceId, Image> m_images;       /**< Loaded images. */
    std::unordered_map<ResourceId, MaterialData> m_materials; /**< Loaded materials. */
    std::unordered_map<ResourceId, std::string> m_strings; /**< Loaded strings. */
    std::unordered_map<ResourceId, Shader> m_shaders;     /**< Loaded shaders. */

    std::unordered_map<std::string, ResourceId>
        m_meshFiles; /**< Maps mesh file to string resource id. */
    std::unordered_map<std::string, ResourceId>
        m_imageFiles; /**< Maps image file to string resource id. */
    std::unordered_map<std::string, ResourceId>
        m_materialFiles; /**< Maps material file to string resource id. */
    std::unordered_map<std::string, ResourceId>
        m_textFiles; /**< Maps text file to string resource id. */
    std::unordered_map<std::string, ResourceId>
        m_shaderFiles; /**< Maps shader program file to shader resource id. */

    std::unordered_map<std::string, MeshData>
        m_pendingMeshes; /**< Preloaded meshes, not published yet. */
    std::unordered_map<std::string, Image>
        m_pendingImages; /**< Preloaded images, not published yet. */

    std::list<IResourceListener*> m_resourceListeners; /**< Registered listeners. */
};

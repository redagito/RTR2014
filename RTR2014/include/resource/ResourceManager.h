#pragma once

#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Image.h"
#include "MaterialData.h"
#include "MeshData.h"
#include "Shader.h"
#include "SoundData.h"
#include "resource/IResourceManager.h"

/**
 * \brief Resource manager implementation.
 */
class ResourceManager : public IResourceManager
{
   public:
    ResourceManager();

    ResourceId createMesh(std::vector<float> vertices, std::vector<unsigned int> indices,
                          std::vector<float> normals, std::vector<float> uvs,
                          PrimitiveType type) override;

    ResourceId loadMesh(const std::string& file) override;

    bool getMesh(ResourceId id, std::vector<float>& vertices, std::vector<unsigned int>& indices,
                 std::vector<float>& normals, std::vector<float>& uvs,
                 PrimitiveType& type) const override;

    bool getMeshBounds(ResourceId id, OrientedBoundingBox& bounds) const override;

    ResourceId createImage(std::vector<unsigned char> imageData, unsigned int width,
                           unsigned int height, ColorFormat format, ColorSpace colorSpace) override;

    ResourceId loadImage(const std::string& file, ColorFormat format,
                         ColorSpace colorSpace) override;

    bool getImage(ResourceId id, std::vector<unsigned char>& data, unsigned int& width,
                  unsigned int& height, ColorFormat& format, ColorSpace& colorSpace) const override;

    ResourceId createMaterial(ResourceId diffuse, ResourceId normal, ResourceId specular,
                              ResourceId glow, ResourceId alpha, ResourceId customShader) override;

    ResourceId loadMaterial(const std::string& file) override;

    bool getMaterial(ResourceId id, ResourceId& diffuse, ResourceId& normal, ResourceId& specular,
                     ResourceId& glow, ResourceId& alpha, ResourceId& customShader) const override;

    ResourceId loadSound(const std::string& file) override;

    const SoundData* getSound(ResourceId id) const override;

    ResourceId createString(const std::string& text) override;

    ResourceId loadString(const std::string& file) override;

    bool getString(ResourceId id, std::string& text) const override;

    ResourceId createShader(const std::string& file, ResourceId vertex, ResourceId tessCtrl,
                            ResourceId tessEval, ResourceId geometry, ResourceId fragment) override;

    ResourceId loadShader(const std::string& file) override;

    bool getShader(ResourceId id, ResourceId& vertex, ResourceId& tessCtrl, ResourceId& tessEval,
                   ResourceId& geometry, ResourceId& fragment) const override;

    bool getShaderFile(ResourceId id, std::string& file) const override;

    void addResourceListener(IResourceListener* listener) override;
    void removeResourceListener(IResourceListener* listener) override;

    /**
     * \brief Loads meshes, the images of the given materials and sounds in parallel.
     *
     * The files are parsed and decoded on worker threads, but not published
     * yet. Publishing happens when the resource is loaded for the first time,
     * which must occur on the thread owning the graphics context.
     */
    void preload(const std::vector<std::string>& meshFiles,
                 const std::vector<std::string>& materialFiles,
                 const std::vector<std::string>& soundFiles);

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

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
     * \brief Decodes a sound file into pcm samples without publishing it.
     */
    static bool decodeSound(const std::string& file, SoundData& data);

    /**
     * \brief Returns the image files referenced by a material file.
     */
    static bool getMaterialImages(const std::string& file, std::vector<ImageDescriptor>& images);

    /**
     * \brief Notifies the listeners about a resource event.
     */
    void notifyResourceListeners(ResourceType type, ResourceId id, ListenerEvent event);

    ResourceId m_nextMeshId;     /**< Next free mesh id. */
    ResourceId m_nextImageId;    /**< Next free image id. */
    ResourceId m_nextMaterialId; /**< Next free material id. */
    ResourceId m_nextStringId;   /**< Next free string id. */
    ResourceId m_nextShaderId;   /**< Next free shader id. */
    ResourceId m_nextSoundId;    /**< Next free sound id. */

    // TODO Change to vector?
    std::unordered_map<ResourceId, MeshData> m_meshes;        /**< Loaded meshes. */
    std::unordered_map<ResourceId, Image> m_images;           /**< Loaded images. */
    std::unordered_map<ResourceId, MaterialData> m_materials; /**< Loaded materials. */
    std::unordered_map<ResourceId, std::string> m_strings;    /**< Loaded strings. */
    std::unordered_map<ResourceId, Shader> m_shaders;         /**< Loaded shaders. */
    std::unordered_map<ResourceId, SoundData> m_sounds;       /**< Loaded sounds. */

    std::unordered_map<std::string, ResourceId>
        m_meshFiles; /**< Maps mesh file to string resource id. */
    std::unordered_map<std::string, ResourceId>
        m_imageFiles; /**< Maps image file to string resource id. */
    std::unordered_map<std::string, ResourceId>
        m_materialFiles; /**< Maps material file to string resource id. */
    std::unordered_map<std::string, ResourceId>
        m_textFiles; /**< Maps text file to string resource id. */
    std::unordered_map<std::string, ResourceId>
        m_shaderFiles; /**< Maps shader program file to string resource id. */
    std::unordered_map<std::string, ResourceId>
        m_soundFiles; /**< Maps sound file to sound resource id. */

    std::unordered_map<std::string, MeshData>
        m_pendingMeshes; /**< Preloaded meshes, not published yet. */
    std::unordered_map<std::string, Image>
        m_pendingImages; /**< Preloaded images, not published yet. */
    std::unordered_map<std::string, SoundData>
        m_pendingSounds; /**< Preloaded sounds, not published yet. */

    std::list<IResourceListener*> m_resourceListeners; /**< Registered listeners. */
};

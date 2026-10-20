#pragma once

#include <functional>
#include <string>
#include <vector>

#include "ResourceConfig.h"
#include "resource/OrientedBoundingBox.h"

class IResourceListener; /**< Listener class. */
struct SoundData;

/**
 * \brief Resource manager interface class.
 * Central resource holder and provider.
 */
class IResourceManager
{
   public:
    /**
     * \brief Virtual destructor for interface class.
     */
    virtual ~IResourceManager();

    /**
     * \brief Creates mesh and returns id.
     */
    [[nodiscard]] virtual ResourceId createMesh(std::vector<float> vertices,
                                                std::vector<unsigned int> indices,
                                                std::vector<float> normals, std::vector<float> uvs,
                                                PrimitiveType type) = 0;

    /**
     * \brief Loads mesh from file.
     */
    [[nodiscard]] virtual ResourceId loadMesh(const std::string& file) = 0;

    /**
     * \brief Retrieves mesh data.
     */
    [[nodiscard]] virtual bool getMesh(ResourceId id, std::vector<float>& vertices,
                                       std::vector<unsigned int>& indices,
                                       std::vector<float>& normals, std::vector<float>& uvs,
                                       PrimitiveType& type) const = 0;

    /**
     * \brief Returns the extent of a mesh in mesh space.
     */
    [[nodiscard]] virtual bool getMeshBounds(ResourceId id, OrientedBoundingBox& bounds) const = 0;

    /**
     * \brief Creates texture object from image data and returns id.
     * \parm imageData  Raw image data.
     * \parm width Image width.
     * \parm height Image height.
     * \parm format Image format of the provided data.
     */
    [[nodiscard]] virtual ResourceId createImage(std::vector<unsigned char> imageData,
                                                 unsigned int width, unsigned int height,
                                                 ColorFormat format) = 0;

    /**
     * \brief Loads image from file.
     */
    [[nodiscard]] virtual ResourceId loadImage(const std::string& file, ColorFormat format) = 0;

    /**
     * \brief Retrieves image data.
     */
    [[nodiscard]] virtual bool getImage(ResourceId id, std::vector<unsigned char>& data,
                                        unsigned int& width, unsigned int& height,
                                        ColorFormat& format) const = 0;

    /**
     * \brief Creates material.
     */
    [[nodiscard]] virtual ResourceId createMaterial(ResourceId diffuseImage, ResourceId normalImage,
                                                    ResourceId specularImage, ResourceId glowImage,
                                                    ResourceId alphaImage,
                                                    ResourceId customShader) = 0;

    /**
     * \brief Loads material from file.
     */
    [[nodiscard]] virtual ResourceId loadMaterial(const std::string& file) = 0;

    /**
     * \brief Returns material data.
     */
    [[nodiscard]] virtual bool getMaterial(ResourceId id, ResourceId& diffuseImage,
                                           ResourceId& normalImage, ResourceId& specularImage,
                                           ResourceId& glowImage, ResourceId& alphaImage,
                                           ResourceId& customShader) const = 0;

    /**
     * \brief Loads a sound file and decodes it into pcm samples.
     * Decoding of preloaded files happens on worker threads; publishing
     * occurs on the thread that first loads the file.
     */
    [[nodiscard]] virtual ResourceId loadSound(const std::string& file) = 0;

    /**
     * \brief Returns the decoded sound data, or null for an unknown id.
     * The pointer stays valid until the resource manager is destroyed.
     */
    virtual const SoundData* getSound(ResourceId id) const = 0;

    /**
     * \brief Creates string resource.
     */
    [[nodiscard]] virtual ResourceId createString(const std::string& text) = 0;

    /**
     * \brief Loads file as string into memory.
     */
    [[nodiscard]] virtual ResourceId loadString(const std::string& file) = 0;

    /**
     * \brief Returns string resource.
     */
    [[nodiscard]] virtual bool getString(ResourceId id, std::string& text) const = 0;

    /**
     * \brief Creates shader resource.
     */
    [[nodiscard]] virtual ResourceId createShader(const std::string& file,
                                                  ResourceId vertexShaderString,
                                                  ResourceId tessellationControlShaderString,
                                                  ResourceId tessellationEvaluationShaderString,
                                                  ResourceId geometryShaderString,
                                                  ResourceId fragmentShaderString) = 0;

    /**
     * \brief Loads shader program from file.
     * The file should usually contain refernces to the shader sources used for the shading stages.
     * For now, .ini files are used.
     * Does not reload already loaded files.
     */
    [[nodiscard]] virtual ResourceId loadShader(const std::string& shaderIniFile) = 0;

    /**
     * \brief Retrieves shader resource.
     */
    [[nodiscard]] virtual bool getShader(ResourceId id, ResourceId& vertexShaderString,
                                         ResourceId& tessellationControlShaderString,
                                         ResourceId& tessellationEvaluationShaderString,
                                         ResourceId& geometryShaderString,
                                         ResourceId& fragmentShaderString) const = 0;

    /**
     * \brief Returns the file a shader program was loaded from.
     */
    [[nodiscard]] virtual bool getShaderFile(ResourceId id, std::string& file) const = 0;

    /**
     * \brief Loads meshes, the images of the given materials and sounds in parallel.
     *
     * The files are parsed and decoded on worker threads, but not published
     * yet. Publishing happens when the resource is loaded for the first time,
     * which must occur on the thread owning the graphics context.
     */
    virtual void preload(const std::vector<std::string>& meshFiles,
                         const std::vector<std::string>& materialFiles,
                         const std::vector<std::string>& soundFiles) = 0;

    /**
     * \brief Adds resource listener.
     */
    virtual void addResourceListener(IResourceListener* listener) = 0;

    /**
     * \brief Removes resource listener.
     */
    virtual void removeResourceListener(IResourceListener* listener) = 0;
};

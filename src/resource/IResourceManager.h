#pragma once

#include <vector>
#include <functional>

/**
 * \brief Resource manager interface class.
 * Central resource holder and provider.
 */
class IResourceManager
{
   public:
    /**
     * \brief Possible listener events.
     */
    enum class EListenerEvent
    {
        Create, /**< Specified resource was created. */
        Change, /**< Specified resource was changed. */
        Delete  /**< Specified resource was deleted. */
    };

    /**
    * \brief Primitive type for vertex data.
    */
    enum class EPrimitiveType
    {
        Point,
        Line,
        Triangle,
        Quad
    };

    /**
    * \brief Color format for texture data.
    */
    enum class EColorFormat
    {
        GreyScale8,
        RGB24,
        RGBA32
    };

    typedef int
        MeshId; /**< Mesh id, -1 is reserved invalid/unused, all other values < -1 are invalid. */
    typedef int
        ImageId; /**< Image id, -1 is reserved invalid/unused, all other values < -1 are invalid. */
    typedef int MaterialId; /**< Material id, -1 is reserved invalid/unused, all other values < -1
                               are invalid. */

    typedef std::function<void(MaterialId, EListenerEvent)> MeshListener;
    typedef std::function<void(MaterialId, EListenerEvent)> ImageListener;
    typedef std::function<void(MaterialId, EListenerEvent)> MaterialListener;

    /**
     * \brief Virtual destructor for interface class.
     */
    virtual ~IResourceManager();

    /**
     * \brief Creates mesh and returns id.
     * TODO It seems that indexed draw in OpenGL cannot handle GL_QUADS primitive, investigate.
     */
    virtual MeshId createMesh(const std::vector<float>& vertices,
                              const std::vector<unsigned int>& indices,
                              const std::vector<float>& normals, const std::vector<float>& uvs,
                              EPrimitiveType type) = 0;

    /**
     * \brief Adds listener to be called on mesh events.
     */
    virtual void addMeshListener(const MeshListener& meshListener) = 0;

    /**
     * \brief Creates texture object from image data and returns id.
     * \parm imageData  Raw image data.
     * \parm width Image width.
     * \parm height Image height.
     * \parm format Image format of the provided data.
     */
    virtual ImageId createImage(const std::vector<unsigned char>& imageData, unsigned int width,
                                unsigned int height, EColorFormat format) = 0;

    /**
     * \brief Adds listener to be called on image events.
     */
    virtual void addImageListener(const ImageListener& imageListener) = 0;

    /**
     * \brief Creates material.
     */
    virtual MaterialId createMaterial(ImageId diffuse, ImageId alpha, ImageId normal,
                                      ImageId specular, ImageId glow) = 0;

    /**
     * \brief Adds listener to be called on material events.
     */
    virtual void addMaterialListener(const MaterialListener& materialListener) = 0;
};

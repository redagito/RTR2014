#pragma once

#include <vector>
#include <functional>

#include "ResourceConfig.h"

class IResourceListener; /**< Listener class. */

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
     * TODO It seems that indexed draw in OpenGL cannot handle GL_QUADS primitive, investigate.
     */
    virtual ResourceId createMesh(const std::vector<float>& vertices,
                                  const std::vector<unsigned int>& indices,
                                  const std::vector<float>& normals, const std::vector<float>& uvs,
                                  EPrimitiveType type) = 0;

    /**
     * \brief Creates texture object from image data and returns id.
     * \parm imageData  Raw image data.
     * \parm width Image width.
     * \parm height Image height.
     * \parm format Image format of the provided data.
     */
    virtual ResourceId createImage(const std::vector<unsigned char>& imageData, unsigned int width,
                                   unsigned int height, EColorFormat format) = 0;

    /**
     * \brief Creates material.
     */
    virtual ResourceId createMaterial(ResourceId diffuseImage, ResourceId alphaImage,
                                      ResourceId normalImage, ResourceId specularImage,
                                      ResourceId glowImage) = 0;

    /**
     * \brief Creates string resource.
     */
    virtual ResourceId createString(const std::string& text) = 0;

    /**
     * \brief Creates shader resource.
     */
    virtual ResourceId createShader(ResourceId vertexShaderString,
                                    ResourceId tessellationControlShaderString,
                                    ResourceId tessellationEvaluationShaderString,
                                    ResourceId geometryShaderString,
                                    ResourceId fragmentShaderString) = 0;

    /**
     * \brief Adds resource listener.
     */
    virtual void addResourceListener(IResourceListener* listener) = 0;

    /**
     * \brief Removes resource listener.
     */
    virtual void removeResourceListener(IResourceListener* listener) = 0;
};

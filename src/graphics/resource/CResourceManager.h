#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <list>
#include <string>

#include "graphics/IResourceManager.h"

#include "SImage.h"
#include "SMaterial.h"
#include "SMesh.h"
#include "SShader.h"

class CResourceManager : public IResourceManager
{
   public:
    CResourceManager();

    ResourceId createMesh(const std::vector<float>& vertices,
                          const std::vector<unsigned int>& indices,
                          const std::vector<float>& normals, const std::vector<float>& uvs,
                          EPrimitiveType type);

    bool getMesh(ResourceId id, std::vector<float>& vertices, std::vector<unsigned int>& indices,
                 std::vector<float>& normals, std::vector<float>& uvs, EPrimitiveType& type) const;

    ResourceId createImage(const std::vector<unsigned char>& imageData, unsigned int width,
                           unsigned int height, EColorFormat format);

    bool getImage(ResourceId id, std::vector<unsigned char>& data, unsigned int& width,
                  unsigned int& height, EColorFormat& format) const;

    ResourceId createMaterial(ResourceId diffuse, ResourceId alpha, ResourceId normal,
                              ResourceId specular, ResourceId glow);

    bool getMaterial(ResourceId id, ResourceId& diffuse, ResourceId& alpha, ResourceId& normal,
                     ResourceId& specular, ResourceId& glow) const;

    ResourceId createString(const std::string& text);

    bool getString(ResourceId id, std::string& text) const;

    ResourceId createShader(ResourceId vertex, ResourceId tessCtrl, ResourceId tessEval,
                            ResourceId geometry, ResourceId fragment);

    bool getShader(ResourceId id, ResourceId& vertex, ResourceId& tessCtrl, ResourceId& tessEval,
                   ResourceId& geometry, ResourceId& fragment) const;

    void addResourceListener(IResourceListener* listener);
    void removeResourceListener(IResourceListener* listener);

   protected:
    void notifyResourceListeners(EResourceType type, ResourceId id, EListenerEvent event);

   private:
    ResourceId m_nextMeshId;     /**< Next free mesh id. */
    ResourceId m_nextImageId;    /**< Next free image id. */
    ResourceId m_nextMaterialId; /**< Next free material id. */
    ResourceId m_nextStringId;   /**< Next free string id. */
    ResourceId m_nextShaderId;   /**< Next free shader id. */

    // TODO Change to vector
    std::unordered_map<ResourceId, SMesh> m_meshes;        /**< Loaded meshes. */
    std::unordered_map<ResourceId, SImage> m_images;       /**< Loaded images. */
    std::unordered_map<ResourceId, SMaterial> m_materials; /**< Loaded materials. */
    std::unordered_map<ResourceId, std::string> m_strings; /**< Loaded strings. */
    std::unordered_map<ResourceId, SShader> m_shaders;     /**< Loaded shaders. */

    std::list<IResourceListener*> m_resourceListeners; /**< Registered listeners. */
};

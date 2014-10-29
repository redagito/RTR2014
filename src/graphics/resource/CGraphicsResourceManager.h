#pragma once

#include <memory>
#include <list>
#include <unordered_map>

#include "resource/IResourceListener.h"
#include "graphics/IGraphicsResourceManager.h"

#include "graphics/resource/CTexture.h"
#include "graphics/resource/CMaterial.h"
#include "graphics/resource/CMesh.h"
#include "graphics/resource/TShaderObject.h"
#include "graphics/resource/CShaderProgram.h"

class CGraphicsResourceManager : public IGraphicsResourceManager, public IResourceListener
{
   public:
    /**
    * \brief Sets resource callbacks.
    */
    CGraphicsResourceManager();

    /**
    * \brief Cleans up resource callbacks.
    */
    ~CGraphicsResourceManager();

    /**
    * \brief On attach callback.
    * Implements the attach callback for the resource listener interface.
    * TODO Currently stubbed, renderer attaches itself to resource manager in constructor.
    */
    void onAttach(IResourceManager* resourceManager);

    /**
    * \brief On detach callback.
    * Implements the detach callback for the resource listener interface.
    * TODO Currently stubbed.
    */
    void onDetach(IResourceManager* resourceManager);

    /**
    * \brief Notify callback.
    * Implements the notify callback for the resource listener interface.
    * The function resolves the resource type and forwards the call to specialized functions.
    */
    void notify(EResourceType type, ResourceId, EListenerEvent event,
                IResourceManager* resourceManager);

    /**
    * \brief Maps id to internal mesh object.
    */
    CMesh* getMesh(ResourceId) const;

    /**
    * \brief Maps id to internal material object.
    */
    CMaterial* getMaterial(ResourceId) const;

    /**
    * \brief Maps id to internal texture object.
    */
    CTexture* getTexture(ResourceId) const;

    /**
    * \brief Maps id to internal shader program object.
    */
    CShaderProgram* getShaderProgram(ResourceId) const;

    /**
    * \brief Returns respective default texture.
    */
    CTexture* getDefaultDiffuseTexture() const;
    CTexture* getDefaultNormalTexture() const;
    CTexture* getDefaultSpecularTexture() const;
    CTexture* getDefaultGlowTexture() const;
    CTexture* getDefaultAlphaTexture() const;

   protected:
    /**
    * \brief Maps id to internal vertex shader object.
    */
    TShaderObject<GL_VERTEX_SHADER>* getVertexShaderObject(ResourceId) const;

    /**
    * \brief Maps id to internal tessellation control shader object.
    */
    TShaderObject<GL_TESS_CONTROL_SHADER>* getTessControlShaderObject(ResourceId) const;

    /**
    * \brief Maps id to internal tessellation evaluation shader object.
    */
    TShaderObject<GL_TESS_EVALUATION_SHADER>* getTessEvalShaderObject(ResourceId) const;

    /**
    * \brief Maps id to internal geometry shader object.
    */
    TShaderObject<GL_GEOMETRY_SHADER>* getGeometryShaderObject(ResourceId) const;

    /**
    * \brief Maps id to internal fragment shader object.
    */
    TShaderObject<GL_FRAGMENT_SHADER>* getFragmentShaderObject(ResourceId) const;

   private:
    /**
    * \brief Initializes default textures for materials.
    */
    void initDefaultTextures();

    /**
    * \brief Loads vertex shader from resource manager.
    */
    bool loadVertexShader(ResourceId id, IResourceManager* resourceManager);

    /**
    * \brief Loads tessellation control shader from resource manager.
    */
    bool loadTessControlShader(ResourceId id, IResourceManager* resourceManager);

    /**
    * \brief Loads tessellation evaluation shader from resource manager.
    */
    bool loadTessEvalShader(ResourceId id, IResourceManager* resourceManager);

    /**
    * \brief Loads geometry shader from resource manager.
    */
    bool loadGeometryShader(ResourceId id, IResourceManager* resourceManager);

    /**
    * \brief Loads fragment shader from resource manager.
    */
    bool loadFragmentShader(ResourceId id, IResourceManager* resourceManager);

    /**
    * \brief Handles resource events for image resources.
    */
    void handleImageEvent(ResourceId, EListenerEvent event, IResourceManager* resourceManager);

    /**
    * \brief Handles resource events for mesh resources.
    */
    void handleMeshEvent(ResourceId, EListenerEvent event, IResourceManager* resourceManager);

    /**
    * \brief Handles resource events for material resources.
    */
    void handleMaterialEvent(ResourceId, EListenerEvent event, IResourceManager* resourceManager);

    /**
    * \brief Handles resource events for shader resources.
    */
    void handleShaderEvent(ResourceId, EListenerEvent event, IResourceManager* resourceManager);

    /**
    * \brief Handles resource events for string resources.
    * Stubbed as the renderer does not need string events.
    * TODO Consider hot reloading of shader source code and on-the-fly
    * recompiling of shader objects/programs
    */
    void handleStringEvent(ResourceId, EListenerEvent event, IResourceManager* resourceManager);

    std::unordered_map<ResourceId, std::unique_ptr<CMesh>>
        m_meshes; /**< Maps mesh id from resource manager to GPU side mesh. */

    std::unordered_map<ResourceId, std::unique_ptr<CTexture>>
        m_textures; /**< Maps image id from resource manager to GPU side texture. */

    std::unordered_map<ResourceId, std::unique_ptr<CMaterial>>
        m_materials; /**< Maps material id from resource manager to cached material. */

    std::unordered_map<ResourceId, std::unique_ptr<TShaderObject<GL_VERTEX_SHADER>>>
        m_vertexShader; /**< Maps string resource ids to compiled vertex shader objects. */

    std::unordered_map<ResourceId, std::unique_ptr<TShaderObject<GL_TESS_CONTROL_SHADER>>>
        m_tessConstrolShader; /**< Maps string resource ids to compiled tessellation control
                                                  shader objects. */

    std::unordered_map<ResourceId, std::unique_ptr<TShaderObject<GL_TESS_EVALUATION_SHADER>>>
        m_tessEvalShader; /**< Maps string resource ids to compiled tessellation evaluation shader
                                          objects. */

    std::unordered_map<ResourceId, std::unique_ptr<TShaderObject<GL_GEOMETRY_SHADER>>>
        m_geometryShader; /**< Maps string resource ids to compiled geometry shader objects. */

    std::unordered_map<ResourceId, std::unique_ptr<TShaderObject<GL_FRAGMENT_SHADER>>>
        m_fragmentShader; /**< Maps string resource ids to compiled fragment shader objects. */

    std::unordered_map<ResourceId, std::unique_ptr<CShaderProgram>>
        m_shaderPrograms; /**< Maps resource ids to linked shader programs. */

    std::unique_ptr<CTexture> m_defaultDiffuseTexture = nullptr;  /**< Default diffuse texture. */
    std::unique_ptr<CTexture> m_defaultNormalTexture = nullptr;   /**< Default normal texture. */
    std::unique_ptr<CTexture> m_defaultSpecularTexture = nullptr; /**< Default specular texture. */
    std::unique_ptr<CTexture> m_defaultGlowTexture = nullptr;     /**< Default glow texture. */
    std::unique_ptr<CTexture> m_defaultAlphaTexture = nullptr;    /**< Default alpha texture. */

    std::list<IResourceManager*>
        m_registeredManagers; /**< Resource managers, this listener is attached to. */
};

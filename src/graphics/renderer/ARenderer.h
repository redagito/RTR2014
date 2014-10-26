#pragma once

#include <memory>
#include <unordered_map>

// Required for inheritance
#include "graphics/IRenderer.h"
#include "resource/IResourceListener.h"

// Resource classes
#include "CMesh.h"
#include "CTexture.h"
#include "CMaterial.h"
#include "TShaderObject.h"
#include "CShaderProgram.h"

class IResourceManager;

/**
* \brief Abstract renderer base class.
* Implements GPU side resource management.
*/
class ARenderer : public IRenderer, IResourceListener
{
   public:
    /**
    * \brief Sets resource callbacks.
    */
    ARenderer(const std::shared_ptr<IResourceManager>& resourceManager);

    /**
    * \brief Cleans up resource callbacks.
    */
    virtual ~ARenderer();

    /**
     * \brief Performs initialization.
     * Implemented in specialized rendering classes.
     */
    virtual bool init() = 0;

    /**
    * \brief Draws whole scene.
    * Implemented in specialized rendering classes.
    */
    virtual void draw(const IScene& scene, const ICamera& camera, const IWindow& window) = 0;

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

   protected:
    /**
    * \brief Performs GL draw call based on mesh data.
    * Sets vertex array object active and performs either indexed or non-indexed draw call.
    * Shader must be set by caller.
    */
    void draw(CMesh* mesh);

    /**
    * \brief Returns resource manager.
    */
    IResourceManager* getResourceManager() const;

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

    // Allow direct access for derived classes
    std::unique_ptr<CTexture> m_defaultDiffuseTexture;  /**< Default diffuse texture. */
    std::unique_ptr<CTexture> m_defaultNormalTexture;   /**< Default normal texture. */
    std::unique_ptr<CTexture> m_defaultSpecularTexture; /**< Default specular texture. */
    std::unique_ptr<CTexture> m_defaultGlowTexture;     /**< Default glow texture. */
    std::unique_ptr<CTexture> m_defaultAlphaTexture;    /**< Default alpha texture. */

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

    std::shared_ptr<IResourceManager> m_resourceManager; /**< Shared resource manager. */

    // TODO Store as arrays?
    // TODO Move resource storage/id mapping to separate class?
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
};

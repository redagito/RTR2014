#pragma once

#include <memory>
#include <list>
#include <unordered_map>
#include <vector>

#include "resource/IResourceListener.h"
#include "graphics/IGraphicsResourceManager.h"

#include "graphics/Texture.h"
#include "graphics/Material.h"
#include "graphics/Mesh.h"
#include "graphics/ShaderObject.h"
#include "graphics/ShaderProgram.h"

class GraphicsResourceManager : public IGraphicsResourceManager, public IResourceListener
{
   public:
    /**
    * \brief Sets resource callbacks.
    */
    GraphicsResourceManager();

    /**
    * \brief Cleans up resource callbacks.
    */
    ~GraphicsResourceManager();

    /**
    * \brief On attach callback.
    * Implements the attach callback for the resource listener interface.
    * TODO Currently stubbed, renderer attaches itself to resource manager in constructor.
    */
    void onAttach(IResourceManager* resourceManager) override;

    /**
     * \brief On detach callback.
     * Implements the detach callback for the resource listener interface.
     * TODO Currently stubbed.
     */
    void onDetach(IResourceManager* resourceManager) override;

    /**
     * \brief Notify callback.
     * Implements the notify callback for the resource listener interface.
     * The function resolves the resource type and forwards the call to specialized functions.
     */
    void notify(ResourceType type, ResourceId, ListenerEvent event,
                IResourceManager* resourceManager) override;

    /**
     * \brief Maps id to internal mesh object.
     */
    Mesh* getMesh(ResourceId) const override;

    /**
     * \brief Maps id to internal material object.
     */
    Material* getMaterial(ResourceId) const override;

    /**
     * \brief Maps id to internal texture object.
     */
    Texture* getTexture(ResourceId) const override;

    /**
     * \brief Maps id to internal shader program object.
     */
    ShaderProgram* getShaderProgram(ResourceId) const override;

    /**
     * \brief Returns respective default texture.
     */
    Texture* getDefaultDiffuseTexture() const override;
    Texture* getDefaultNormalTexture() const override;
    Texture* getDefaultSpecularTexture() const override;
    Texture* getDefaultGlowTexture() const override;
    Texture* getDefaultAlphaTexture() const override;

   protected:
    /**
    * \brief Maps id to internal vertex shader object.
    */
    ShaderObject<GL_VERTEX_SHADER>* getVertexShaderObject(ResourceId) const;

    /**
    * \brief Maps id to internal tessellation control shader object.
    */
    ShaderObject<GL_TESS_CONTROL_SHADER>* getTessControlShaderObject(ResourceId) const;

    /**
    * \brief Maps id to internal tessellation evaluation shader object.
    */
    ShaderObject<GL_TESS_EVALUATION_SHADER>* getTessEvalShaderObject(ResourceId) const;

    /**
    * \brief Maps id to internal geometry shader object.
    */
    ShaderObject<GL_GEOMETRY_SHADER>* getGeometryShaderObject(ResourceId) const;

    /**
    * \brief Maps id to internal fragment shader object.
    */
    ShaderObject<GL_FRAGMENT_SHADER>* getFragmentShaderObject(ResourceId) const;

   private:
    /**
    * \brief Initializes default textures for materials.
    */
    void initDefaultTextures();

    /**
    * \brief Handles resource events for image resources.
    */
    void handleImageEvent(ResourceId, ListenerEvent event, IResourceManager* resourceManager);

    /**
    * \brief Handles resource events for mesh resources.
    */
    void handleMeshEvent(ResourceId, ListenerEvent event, IResourceManager* resourceManager);

    /**
    * \brief Handles resource events for material resources.
    */
    void handleMaterialEvent(ResourceId, ListenerEvent event, IResourceManager* resourceManager);

    /**
    * \brief Handles resource events for shader resources.
    */
    void handleShaderEvent(ResourceId, ListenerEvent event, IResourceManager* resourceManager);

    /**
    * \brief Handles resource events for string resources.
    * Stubbed as the renderer does not need string events.
    * TODO Consider hot reloading of shader source code and on-the-fly
    * recompiling of shader objects/programs
    */
    void handleStringEvent(ResourceId, ListenerEvent event, IResourceManager* resourceManager);

    std::unordered_map<ResourceId, std::unique_ptr<Mesh>>
        m_meshes; /**< Maps mesh id from resource manager to GPU side mesh. */

    std::unordered_map<ResourceId, std::unique_ptr<Texture>>
        m_textures; /**< Maps image id from resource manager to GPU side texture. */

    std::unordered_map<ResourceId, std::unique_ptr<Material>>
        m_materials; /**< Maps material id from resource manager to cached material. */

    std::unordered_map<ResourceId, std::unique_ptr<ShaderObject<GL_VERTEX_SHADER>>>
        m_vertexShader; /**< Maps string resource ids to compiled vertex shader objects. */

    std::unordered_map<ResourceId, std::unique_ptr<ShaderObject<GL_TESS_CONTROL_SHADER>>>
        m_tessConstrolShader; /**< Maps string resource ids to compiled tessellation control
                                                  shader objects. */

    std::unordered_map<ResourceId, std::unique_ptr<ShaderObject<GL_TESS_EVALUATION_SHADER>>>
        m_tessEvalShader; /**< Maps string resource ids to compiled tessellation evaluation shader
                                          objects. */

    std::unordered_map<ResourceId, std::unique_ptr<ShaderObject<GL_GEOMETRY_SHADER>>>
        m_geometryShader; /**< Maps string resource ids to compiled geometry shader objects. */

    std::unordered_map<ResourceId, std::unique_ptr<ShaderObject<GL_FRAGMENT_SHADER>>>
        m_fragmentShader; /**< Maps string resource ids to compiled fragment shader objects. */

    std::unordered_map<ResourceId, std::unique_ptr<ShaderProgram>>
        m_shaderPrograms; /**< Maps resource ids to linked shader programs. */

    std::unique_ptr<Texture> m_defaultDiffuseTexture = nullptr;  /**< Default diffuse texture. */
    std::unique_ptr<Texture> m_defaultNormalTexture = nullptr;   /**< Default normal texture. */
    std::unique_ptr<Texture> m_defaultSpecularTexture = nullptr; /**< Default specular texture. */
    std::unique_ptr<Texture> m_defaultGlowTexture = nullptr;     /**< Default glow texture. */
    std::unique_ptr<Texture> m_defaultAlphaTexture = nullptr;    /**< Default alpha texture. */

    std::list<IResourceManager*>
        m_registeredManagers; /**< Resource managers, this listener is attached to. */
};

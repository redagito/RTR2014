#pragma once

#include <list>
#include <memory>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include "graphics/IRenderer.h"
#include "graphics/IResourceListener.h"
#include "graphics/ResourceConfig.h"

#include "CMesh.h"
#include "CTexture.h"
#include "CMaterial.h"
#include "TShaderObject.h"
#include "CShaderProgram.h"
#include "CFrameBuffer.h"

#include "SRenderRequest.h"

class IResourceManager;

/**
 * \brief Renderer implementation for deferred rendering.
 * The renderer registers itself for resource events.
 */
class CRenderer : public IRenderer, IResourceListener
{
   public:
    /**
     * \brief Sets resource callbacks.
     */
    CRenderer(const std::shared_ptr<IResourceManager>& resourceManager);

    /**
     * \brief Cleans up resource callbacks.
     */
    ~CRenderer();

    /**
     * \brief Draws whole scene.
     * Implements the draw call as specified by the renderer interface.
     * This function implements the highlevel drawing logic for the scene,
     * selects rendering targets and manages render passes.
     */
    void draw(const IScene& scene, const ICamera& camera, const IWindow& window);

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
    * \brief Utility draw function.
    * Resolves the resource ids to internal objects, constructs
    * the transformation matrices and forwards call.
    */
    void draw(ResourceId mesh, const glm::vec3& position, const glm::vec3& rotation,
              const glm::vec3& scale, ResourceId);

    /**
    * \brief Draws mesh to current rendering target.
    * Selects the rendering method (indexed, direct, etc.) based on the available mesh data
    * and performs actual draw call.
    */
    void draw(CMesh* mesh, const glm::mat4& translation, const glm::mat4& rotation,
              const glm::mat4& scale, CMaterial* material);

	/**
	* \brief Draws meshes with custom shaders set in their materials.
	*/
	void drawCustom();

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

   private:
	void initDefaultShaders();
	void initDefaultTextures();
	void initFrameBuffer();

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

	CShaderProgram* m_defaultShader; /**< Default shader for rendering. TODO for testing, remove later. */
	CTexture* m_defaultNormalTexture;
	CTexture* m_defaultSpecularTexture;
	CTexture* m_defaultGlowTexture;
	CTexture* m_defaultAlphaTexture;

	std::list<SRenderRequest> m_customShaderMeshes; /**< Render requests with custom shaders. */

	std::shared_ptr<CFrameBuffer> m_gBuffer; /**< GBuffer frame buffer. */
};

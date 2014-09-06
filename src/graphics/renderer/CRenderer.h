#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include <glm/glm.hpp>

#include "graphics/IRenderer.h"
#include "graphics/IResourceListener.h"
#include "graphics/ResourceConfig.h"

#include "CMesh.h"
#include "CTexture.h"
#include "CMaterial.h"
#include "TShaderObject.h"
#include "CShaderProgram.h"

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
     */
    void draw(const IScene& scene, const ICamera& camera, const IWindow& window);

    /**
     * \brief Draws single mesh.
     * Resolves ids to objects and forwards call.
     */
    void draw(ResourceId mesh, const glm::vec3& position, const glm::vec3& rotation,
              const glm::vec3& scale, ResourceId);

    /**
     * \brief Draws mesh to current rendering target.
     */
    void draw(const CMesh* mesh, const glm::mat4& translation, const glm::mat4& rotation,
              const glm::mat4& scale, CMaterial* material);

    void onAttach(IResourceManager* resourceManager);
    void onDetach(IResourceManager* resourceManager);
    void notify(EResourceType type, ResourceId, EListenerEvent event,
                IResourceManager* resourceManager);

   protected:
    /**
     * \brief Maps id to internal resource object.
     */
    CMesh* getMesh(ResourceId) const;
    CMaterial* getMaterial(ResourceId) const;
    CTexture* getTexture(ResourceId) const;
    CShaderProgram* getShaderProgram(ResourceId) const;

    TShaderObject<GL_VERTEX_SHADER>* getVertexShaderObject(ResourceId) const;
    TShaderObject<GL_TESS_CONTROL_SHADER>* getTessControlShaderObject(ResourceId) const;
    TShaderObject<GL_TESS_EVALUATION_SHADER>* getTessEvalShaderObject(ResourceId) const;
    TShaderObject<GL_GEOMETRY_SHADER>* getGeometryShaderObject(ResourceId) const;
    TShaderObject<GL_FRAGMENT_SHADER>* getFragmentShaderObject(ResourceId) const;

    bool loadVertexShader(ResourceId id, IResourceManager* resourceManager);
    bool loadTessControlShader(ResourceId id, IResourceManager* resourceManager);
    bool loadTessEvalShader(ResourceId id, IResourceManager* resourceManager);
    bool loadFGeometryShader(ResourceId id, IResourceManager* resourceManager);
    bool loadFragmentShader(ResourceId id, IResourceManager* resourceManager);

    void handleImageEvent(ResourceId, EListenerEvent event, IResourceManager* resourceManager);
    void handleMeshEvent(ResourceId, EListenerEvent event, IResourceManager* resourceManager);
    void handleMaterialEvent(ResourceId, EListenerEvent event, IResourceManager* resourceManager);
    void handleShaderEvent(ResourceId, EListenerEvent event, IResourceManager* resourceManager);
    void handleStringEvent(ResourceId, EListenerEvent event, IResourceManager* resourceManager);

   private:
    std::shared_ptr<IResourceManager> m_resourceManager; /**< Shared resource manager. */

    // TODO Store as arrays?
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

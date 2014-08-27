#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include <glm/glm.hpp>

#include "graphics/IRenderer.h"
#include "graphics/IResourceManager.h"
#include "graphics/IResourceListener.h"

#include "CMesh.h"
#include "CTexture.h"
#include "CMaterial.h"

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
    void draw(IResourceManager::ResourceId mesh, const glm::vec3& position,
              const glm::vec3& rotation, const glm::vec3& scale, IResourceManager::ResourceId);

    /**
     * \brief Draws mesh to current rendering target.
     */
    void draw(const std::unique_ptr<CMesh>& mesh, const glm::mat4& translation,
              const glm::mat4& rotation, const glm::mat4& scale,
              const std::unique_ptr<CMaterial>& material);

    void onAttach(IResourceManager* resourceManager);
    void onDetach(IResourceManager* resourceManager);
    void notify(IResourceManager::EResourceType type, IResourceManager::ResourceId,
                IResourceManager::EListenerEvent event, IResourceManager* resourceManager);

   protected:
    /**
     * \brief Maps mesh id to internal mesh object.
     */
    const std::unique_ptr<CMesh>& getMesh(IResourceManager::ResourceId);

    /**
     * \brief Maps material id to internal material object.
     */
    const std::unique_ptr<CMaterial>& getMaterial(IResourceManager::ResourceId);

   private:
    std::shared_ptr<IResourceManager> m_resourceManager; /**< Shared resource manager. */

    // TODO Store as arrays?
    std::unordered_map<IResourceManager::ResourceId, std::unique_ptr<CMesh>>
        m_meshes; /**< Maps mesh id from resource manager to GPU side mesh. */
    std::unordered_map<IResourceManager::ResourceId, std::unique_ptr<CTexture>>
        m_textures; /**< Maps image id from resource manager to GPU side texture. */
    std::unordered_map<IResourceManager::ResourceId, std::unique_ptr<CMaterial>>
        m_materials; /**< Maps material id from resource manager to cached material. */
};

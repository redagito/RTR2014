#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include <glm/glm.hpp>

#include "graphics/IRenderer.h"
#include "resource/IResourceManager.h"

#include "CMesh.h"
#include "CTexture.h"
#include "CMaterial.h"

/**
 * \brief Renderer implementation for deferred rendering.
 */
class CRenderer : public IRenderer
{
   public:
    CRenderer(const std::shared_ptr<IResourceManager>& resourceManager);

    void draw(const IScene& scene, const ICamera& camera, const IWindow& window);

    /**
 * \brief Draw call for mesh.
 */
    void draw(IResourceManager::MeshId mesh, const glm::mat4& translation,
              const glm::mat4& rotation, const glm::mat4& scale, IResourceManager::MaterialId);

    void draw(const std::unique_ptr<CMesh>& mesh, const glm::mat4& translation,
              const glm::mat4& rotation, const glm::mat4& scale,
              const std::unique_ptr<CMaterial>& material);

   private:
    std::shared_ptr<IResourceManager> m_resourceManager; /**< Shared resource manager. */

    // TODO Store as arrays?
    std::unordered_map<IResourceManager::MeshId, std::unique_ptr<CMesh>>
        m_meshes; /**< Maps mesh id from resource manager to GPU side mesh. */
    std::unordered_map<IResourceManager::ImageId, std::unique_ptr<CTexture>>
        m_textures; /**< Maps image id from resource manager to GPU side texture. */
    std::unordered_map<IResourceManager::MaterialId, std::unique_ptr<CMaterial>>
        m_materials; /**< Maps material id from resource manager to cached material. */
};

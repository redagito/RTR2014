#include "CRenderer.h"

#include "graphics/IScene.h"
#include "graphics/ICamera.h"
#include "graphics/IWindow.h"

CRenderer::CRenderer(const std::shared_ptr<IResourceManager>& resourceManager)
    : m_resourceManager(resourceManager)
{
    return;
}

void CRenderer::draw(const IScene& scene, const ICamera& camera, const IWindow& window)
{
    window.setActive();
    // TODO Rendering logic
}

void CRenderer::draw(IResourceManager::MeshId mesh, const glm::mat4& translation,
                     const glm::mat4& rotation, const glm::mat4& scale,
                     IResourceManager::MaterialId)
{
}

void CRenderer::draw(const std::unique_ptr<CMesh>& mesh, const glm::mat4& translation,
                     const glm::mat4& rotation, const glm::mat4& scale,
                     const std::unique_ptr<CMaterial>& material)
{
    // TODO Rendering logic
}

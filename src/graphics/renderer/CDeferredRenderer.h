#pragma once

#include <memory>
#include <list>

// Required by inheritance
#include "ARenderer.h"

#include "CFrameBuffer.h"
#include "SRenderRequest.h"

/**
* \brief Deferred renderer implementation.
*/
class CDeferredRenderer : public ARenderer
{
   public:
    CDeferredRenderer(const std::shared_ptr<IResourceManager>& resourceManager);
    ~CDeferredRenderer();

    bool init();

    void draw(const IScene& scene, const ICamera& camera, const IWindow& window);

   protected:
    bool initDefaultShaders();

    void draw(ResourceId meshId, const glm::vec3& position, const glm::vec3& rotation,
              const glm::vec3& scale, ResourceId materialId);
    void draw(CMesh* mesh, const glm::mat4& translation, const glm::mat4& rotation,
              const glm::mat4& scale, CMaterial* material);

   private:
    CFrameBuffer m_frameBuffer;
    std::shared_ptr<CTexture> m_colorTexture;
    std::shared_ptr<CTexture> m_normalTexture;

    glm::mat4 m_currentView;       /**< Stores the current view matrix. */
    glm::mat4 m_currentProjection; /**< Stores the current projection matrix. */

    std::list<SRenderRequest> m_customShaderMeshes; /**< Render requests with custom shaders. */
    CShaderProgram*
        m_defaultShader; /**< Default shader for rendering. TODO for testing, remove later. */
};

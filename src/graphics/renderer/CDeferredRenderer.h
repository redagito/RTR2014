#pragma once

#include <memory>
#include <list>

// Required by inheritance
#include "ARenderer.h"

#include "CFrameBuffer.h"
#include "SRenderRequest.h"
#include "resource/ResourceConfig.h"

#include "pass/CScreenQuadPass.h"

class CShaderProgram;
class IResourceManager;

/**
* \brief Deferred renderer implementation.
*/
class CDeferredRenderer : public ARenderer
{
   public:
    CDeferredRenderer();
    ~CDeferredRenderer();

    bool init(IResourceManager* manager);

    void draw(const IScene& scene, const ICamera& camera, const IWindow& window,
              const IGraphicsResourceManager& manager);

	static CDeferredRenderer* create(IResourceManager* manager);

   protected:
	bool initShaders(IResourceManager* manager);

    void draw(CMesh* mesh, const glm::mat4& translation, const glm::mat4& rotation,
              const glm::mat4& scale, CMaterial* material, const IGraphicsResourceManager& manager);

   private:
    // Geometry pass
    CFrameBuffer m_frameBuffer;
    std::shared_ptr<CTexture> m_depthTexture;
    std::shared_ptr<CTexture> m_diffuseTexture;
    std::shared_ptr<CTexture> m_normalTexture;
    GLenum m_drawBuffers[2];
	ResourceId m_geometryPassShaderId = -1;
    CShaderProgram* m_geometryPassShader = nullptr;

    glm::mat4 m_currentView;       /**< Stores the current view matrix. */
    glm::mat4 m_currentProjection; /**< Stores the current projection matrix. */

	CScreenQuadPass m_screenQuadPass;
    std::list<SRenderRequest> m_customShaderMeshes; /**< Render requests with custom shaders. */
};

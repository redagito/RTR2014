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
    /**
    * \brief Initializes resources for geometry pass.
    */
    bool initGeometryPass(IResourceManager* manager);

    /**
    * \brief Initializes resources for point light pass.
    */
    bool initPointLightPass(IResourceManager* manager);

    void draw(CMesh* mesh, const glm::mat4& translation, const glm::mat4& rotation,
              const glm::mat4& scale, CMaterial* material, const IGraphicsResourceManager& manager);

   private:
    glm::mat4 m_currentView;       /**< Stores the current view matrix. */
    glm::mat4 m_currentProjection; /**< Stores the current projection matrix. */

    // Geometry pass
    CFrameBuffer m_geometryBuffer;                  /**< GBuffer. */
    std::shared_ptr<CTexture> m_depthTexture;       /**< Depth texture attachment. */
    std::shared_ptr<CTexture> m_diffuseGlowTexture; /**< Diffuse texture with glow as alpha. */
    std::shared_ptr<CTexture>
        m_normalSpecularTexture; /**< Normal texture with specularity as alpha. */
    GLenum m_geometryDrawBuffers[2];
    ResourceId m_geometryPassShaderId = -1;
    CShaderProgram* m_geometryPassShader = nullptr;

    // Light pass
    CFrameBuffer m_lightPassFrameBuffer; /**< Stores lit scene and depth. */
    std::shared_ptr<CTexture> m_colorTexture;
    GLenum m_pointLightDrawBuffers[1];
    ResourceId m_pointLightPassShaderId = -1;
    CShaderProgram* m_pointLightPassShader = nullptr;
    ResourceId m_pointLightSphereId = -1;

    CScreenQuadPass m_screenQuadPass;
    std::list<SRenderRequest> m_customShaderMeshes; /**< Render requests with custom shaders. */
};

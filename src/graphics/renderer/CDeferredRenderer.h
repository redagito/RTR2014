#pragma once

#include <memory>
#include <list>

// Required by inheritance
#include "ARenderer.h"

#include "CFrameBuffer.h"
#include "SRenderRequest.h"
#include "CTransformer.h"

#include "resource/ResourceConfig.h"

#include "pass/CScreenQuadPass.h"

class CShaderProgram;
class IResourceManager;
class ISceneQuery;

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
    * \brief Writes geometry data into g-buffer.
    */
    void geometryPass(const IScene& scene, const ICamera& camera, const IWindow& window,
                      const IGraphicsResourceManager& manager, ISceneQuery& query);

    /**
    * \brief Performs shadow map calculation.
    */
    void shadowMapPass(const IScene& scene, const ICamera& camera, const IWindow& window,
                       const IGraphicsResourceManager& manager);

    /**
    * \brief Writes light data into l-buffer.
    */
    void lightPass(const IScene& scene, const ICamera& camera, const IWindow& window,
                   const IGraphicsResourceManager& manager, ISceneQuery& query);

    /**
    * \brief Writes point light data to l-buffer.
    */
    void pointLightPass(const IScene& scene, const ICamera& camera, const IWindow& window,
                        const IGraphicsResourceManager& manager, ISceneQuery& query);

    /**
    * \brief Writes directional light data to l-buffer.
    */
    void directionalLightPass(const IScene& scene, const ICamera& camera, const IWindow& window,
                              const IGraphicsResourceManager& manager, ISceneQuery& query);

    /**
    * \brief Performs scene illumination and tone mapping.
    */
    void illuminationPass(const IScene& scene, const ICamera& camera, const IWindow& window,
                          const IGraphicsResourceManager& manager, ISceneQuery& query);

    /**
    * \brief Performs post processing of lit scene.
    */
    void postProcessPass(const IScene& scene, const ICamera& camera, const IWindow& window,
                         const IGraphicsResourceManager& manager, ISceneQuery& query);

    /**
    * \brief Draws scene texture to main FBO.
    */
    void displayPass(const IWindow& window, const IGraphicsResourceManager& manager,
                     std::shared_ptr<CTexture>& texture);

    /**
    * \brief Initializes resources for geometry pass.
    */
    bool initGeometryPass(IResourceManager* manager);

    /**
    * \brief Initializes resources for point light pass.
    */
    bool initPointLightPass(IResourceManager* manager);

    /**
    * \brief Initializes resources for directional light pass.
    */
    bool initDirectionalLightPass(IResourceManager* manager);

    /**
    * \brief Initializes resources for illumination pass.
    */
    bool initIlluminationPass(IResourceManager* manager);

    /**
     * \brief Initializes resources for shadow map pass
     */
    bool initShadowMapPass(IResourceManager* manager);

    /**
     * \brief Initializes post processing pass.
     */
    bool initPostProcessPass(IResourceManager* manager);

    /**
    * \brief Initializes post process pass for depth of field.
    *
    * Called by initPostProcesPass.
    */
    bool initDepthOfFieldPass(IResourceManager* manager);

    /**
    * \brief Display pass draws final image to screen.
    */
    bool initDisplayPass(IResourceManager* manager);

    void draw(CMesh* mesh, const glm::mat4& translation, const glm::mat4& rotation,
              const glm::mat4& scale, CMaterial* material, const IGraphicsResourceManager& manager,
              CShaderProgram* shader);

   private:
    CTransformer m_transformer; /**< Stores current transformation matrices. */

    // Geometry pass
    // TODO Put into geometry pass class
    CFrameBuffer m_geometryBuffer;                      /**< GBuffer. */
    std::shared_ptr<CTexture> m_depthTexture = nullptr; /**< Depth texture attachment. */
    std::shared_ptr<CTexture> m_diffuseGlowTexture =
        nullptr; /**< Diffuse texture with glow as alpha. */
    std::shared_ptr<CTexture>
        m_normalSpecularTexture; /**< Normal texture with specularity as alpha. */
    ResourceId m_geometryPassShaderId = -1;

    // Shadow map pass
    CFrameBuffer m_shadowMapBuffer;
    std::shared_ptr<CTexture> m_shadowDepthTexture = nullptr;
    ResourceId m_shadowMapPassShaderId = -1;
    CShaderProgram* m_shadowMapPassShader = nullptr;

    // Light pass common resources
    // TODO Put into light pass class
    CFrameBuffer m_lightPassFrameBuffer; /**< Stores lit scene and depth. */
    std::shared_ptr<CTexture> m_lightPassTexture = nullptr;

    // Point light pass
    ResourceId m_pointLightPassShaderId = -1;
    ResourceId m_pointLightSphereId = -1;

    // Directional light pass
    ResourceId m_directionalLightPassShaderId = -1;
    ResourceId m_directionalLightScreenQuadId = -1;

    // Illumination pass
    ResourceId m_illuminationPassShaderId = -1;
    ResourceId m_illuminationPassScreenQuadId = -1;
    CFrameBuffer m_illumationPassFrameBuffer;
    std::shared_ptr<CTexture> m_illuminationPassTexture = nullptr;

    // Post processing pass
    ResourceId m_postProcessScreenQuadId = -1;
    CFrameBuffer m_postProcessPassFrameBuffer;
    std::shared_ptr<CTexture> m_postProcessPassTexture = nullptr;

    // Gauss blur pass
    ResourceId m_gaussBlurVerticalShaderId = -1;
    ResourceId m_gaussBlurHorizontalShaderId = -1;

    // Depth-of-field pass
    ResourceId m_depthOfFieldPassShaderId = -1;
    float m_focusNear = 1.f;
    float m_focusFar = 10.f;
    float m_blurNear = 0.1f;
    float m_blurFar = 20.f;

    // Display pass for final screen draw
    ResourceId m_displayPassShaderId = -1;

    // Fullscreen draw pass
    CScreenQuadPass m_screenQuadPass;
    std::list<SRenderRequest> m_customShaderMeshes; /**< Render requests with custom shaders. */
};

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
    void geometryPass(const IScene& scene, const ICamera& camera, const IWindow& window,
                      const IGraphicsResourceManager& manager, ISceneQuery& query);

    void shadowMapPass(const IScene& scene, const ICamera& camera, const IWindow& window,
                       const IGraphicsResourceManager& manager);

    void lightPass(const IScene& scene, const ICamera& camera, const IWindow& window,
                   const IGraphicsResourceManager& manager, ISceneQuery& query);

    void pointLightPass(const IScene& scene, const ICamera& camera, const IWindow& window,
                        const IGraphicsResourceManager& manager, ISceneQuery& query);

    void directionalLightPass(const IScene& scene, const ICamera& camera, const IWindow& window,
                              const IGraphicsResourceManager& manager, ISceneQuery& query);

    void illuminationPass(const IScene& scene, const ICamera& camera, const IWindow& window,
                          const IGraphicsResourceManager& manager, ISceneQuery& query);

    void postProcessPass(const IScene& scene, const ICamera& camera, const IWindow& window,
                         const IGraphicsResourceManager& manager, ISceneQuery& query);

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

    void draw(CMesh* mesh, const glm::mat4& translation, const glm::mat4& rotation,
              const glm::mat4& scale, CMaterial* material, const IGraphicsResourceManager& manager,
              CShaderProgram* shader);

   private:
    // Stores current transformation matrices
    CTransformer m_transformer;

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
	CFrameBuffer m_illimationPassFrameBuffer;
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

    // Fullscreen draw pass
    CScreenQuadPass m_screenQuadPass;
    std::list<SRenderRequest> m_customShaderMeshes; /**< Render requests with custom shaders. */
};

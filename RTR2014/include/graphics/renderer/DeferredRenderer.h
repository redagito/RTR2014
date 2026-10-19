#pragma once

#include <memory>

// Required by inheritance
#include "Renderer.h"

#include "FrameBuffer.h"
#include "ShadowCubePass.h"
#include "Transformer.h"

#include "resource/ResourceConfig.h"


class Material;
class ShaderProgram;
class IResourceManager;
class ISceneQuery;

/**
* \brief Deferred renderer implementation.
*/
class DeferredRenderer : public Renderer
{
   public:
    DeferredRenderer();
    ~DeferredRenderer();

    bool init(IResourceManager* manager);

    void draw(const IScene& scene, const ICamera& camera, const IWindow& window,
              const IGraphicsResourceManager& manager);

    static DeferredRenderer* create(IResourceManager* manager);

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
    void postProcessPass(const ICamera& camera, const IWindow& window,
                         const IGraphicsResourceManager& manager,
                         const std::shared_ptr<Texture>& texture);

    void fogPass(const ICamera& camera, const IWindow&, const IGraphicsResourceManager& manager,
                 const std::shared_ptr<Texture>& texture);

    /**
    * \brief FXAA Pass.
    */
    void fxaaPass(const IWindow& window, const IGraphicsResourceManager& manager,
                  const std::shared_ptr<Texture>& texture);

    /**
    * \brief Vertical gauss blur pass.
    */
    void gaussBlurVerticalPass(const IWindow& window, const IGraphicsResourceManager& manager,
                               const std::shared_ptr<Texture>& texture);

    /**
    * \brief Horizontal gauss blur pass.
    */
    void gaussBlurHorizontalPass(const IWindow& window, const IGraphicsResourceManager& manager,
                                 const std::shared_ptr<Texture>& texture);

    void depthOfFieldPass(const ICamera& camera, const IWindow& window,
                          const IGraphicsResourceManager& manager,
                          const std::shared_ptr<Texture>& sceneTexture,
                          const std::shared_ptr<Texture>& blurTexture);

    /**
    * \brief Draws scene texture to main FBO.
    */
    void displayPass(const IWindow& window, const IGraphicsResourceManager& manager,
                     const std::shared_ptr<Texture>& texture);

    void passthroughPass(const IWindow& window, const IGraphicsResourceManager& manager,
                         const std::shared_ptr<Texture>& texture);

    /**
    * \brief Depth visualization pass.
    *
    * For debugging.
    */
    void visualizeDepthPass(const ICamera& camera, const IWindow& window,
                            const IGraphicsResourceManager& manager);

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
    * \brief Initializes resources for post processing pass.
    */
    bool initPostProcessPass(IResourceManager* manager);

    /**
    * \brief Initializes post process pass for depth of field.
    *
    * Called by initPostProcesPass.
    */
    bool initDepthOfFieldPass(IResourceManager* manager);

    /**
    * \brief Initializes vertical gauss blur pass.
    */
    bool initGaussBlurVerticalPass(IResourceManager* manager);

    /**
    * \brief Initializes horizontal gauss blur pass.
    */
    bool initGaussBlurHorizontalPass(IResourceManager* manager);

    /**
    * \brief Initializes FXAA pass for post processing.
    */
    bool initFxaaPass(IResourceManager* manager);

    /**
    * \brief Initializes fog pass for post processing.
    */
    bool initFogPass(IResourceManager* manager);

    /**
    * \brief Display pass draws final image to screen.
    */
    bool initDisplayPass(IResourceManager* manager);

    bool initVisualizeDepthPass(IResourceManager* manager);

    void draw(Mesh* mesh, const glm::mat4& translation, const glm::mat4& rotation,
              const glm::mat4& scale, Material* material, const IGraphicsResourceManager& manager,
              ShaderProgram* shader);

   private:
    Transformer m_transformer; /**< Stores current transformation matrices. */

    // Geometry pass
    // TODO Put into geometry pass class
    FrameBuffer m_geometryBuffer;                      /**< GBuffer. */
    std::shared_ptr<Texture> m_depthTexture = nullptr; /**< Depth texture attachment. */
    std::shared_ptr<Texture> m_diffuseGlowTexture =
        nullptr; /**< Diffuse texture with glow as alpha. */
    std::shared_ptr<Texture>
        m_normalSpecularTexture; /**< Normal texture with specularity as alpha. */
    ResourceId m_geometryPassShaderId = -1;

    // Shadow map pass
    ResourceId m_shadowMapPassShaderId = -1;
    ShaderProgram* m_shadowMapPassShader = nullptr;
    FrameBuffer m_shadowMapBuffer;
    std::shared_ptr<Texture> m_shadowDepthTexture = nullptr;

    // Shadow cube pass
    ShadowCubePass m_shadowCubePass;

    // Light pass common resources
    // TODO Put into light pass class
    FrameBuffer m_lightPassFrameBuffer; /**< Stores lit scene and depth. */
    std::shared_ptr<Texture> m_lightPassTexture = nullptr;

    // Point light pass
    ResourceId m_pointLightPassShaderId = -1;
    ResourceId m_pointLightSphereId = -1;

    // Directional light pass
    ResourceId m_directionalLightPassShaderId = -1;
    ResourceId m_directionalLightScreenQuadId = -1;

    // Illumination pass
    ResourceId m_illuminationPassShaderId = -1;
    ResourceId m_illuminationPassScreenQuadId = -1;
    FrameBuffer m_illumationPassFrameBuffer;
    std::shared_ptr<Texture> m_illuminationPassTexture = nullptr;

    // Post processing pass
    ResourceId m_postProcessScreenQuadId = -1;
    FrameBuffer m_postProcessPassFrameBuffer0;
    FrameBuffer m_postProcessPassFrameBuffer1;
    FrameBuffer m_postProcessPassFrameBuffer2;

    std::shared_ptr<Texture> m_postProcessPassTexture0 = nullptr;
    std::shared_ptr<Texture> m_postProcessPassTexture1 = nullptr;
    std::shared_ptr<Texture> m_postProcessPassTexture2 = nullptr;
    std::shared_ptr<Texture> m_postProcessPassOutputTexture = nullptr;

    // Gauss blur pass
    ResourceId m_gaussBlurVerticalShaderId = -1;
    ResourceId m_gaussBlurHorizontalShaderId = -1;

    // FXAA pass
    ResourceId m_fxaaPassShaderId = -1;

    // Fog pass
    ResourceId m_fogPassShaderId = -1;

    // Depth-of-field pass
    ResourceId m_depthOfFieldPassShaderId = -1;

    // Display pass for final screen draw
    ResourceId m_displayPassShaderId = -1;

    // Depth visualization pass
    ResourceId m_visualizeDepthPassShaderId = -1;

    // Fullscreen draw pass
};

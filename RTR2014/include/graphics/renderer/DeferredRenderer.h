#pragma once

#include <memory>

// Required by inheritance
#include "Renderer.h"

#include "DepthVisualizationPass.h"
#include "DisplayPass.h"
#include "FrameBuffer.h"
#include "GeometryPass.h"
#include "PostProcessPass.h"
#include "ShadowCubePass.h"
#include "ShadowMapPass.h"
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

   private:
    Transformer m_transformer; /**< Stores current transformation matrices. */

    // Geometry pass
    GeometryPass m_geometryPass;

    // Shadow map pass
    ShadowMapPass m_shadowMapPass;

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
    PostProcessPass m_postProcessPass;

    // Display pass for final screen draw
    DisplayPass m_displayPass;

    // Depth visualization pass
    DepthVisualizationPass m_depthVisualizationPass;

    // Fullscreen draw pass
};

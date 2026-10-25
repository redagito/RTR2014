#pragma once

#include <memory>

// Required by inheritance
#include "DepthVisualizationPass.h"
#include "DisplayPass.h"
#include "GeometryPass.h"
#include "IlluminationPass.h"
#include "LightPass.h"
#include "MeteringPass.h"
#include "PostProcessPass.h"
#include "Renderer.h"
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

    const char* getName() const override;

    bool init(IResourceManager* manager, const std::string& skyCubemapFile = std::string());

    void draw(const IScene& scene, const ICamera& camera, const IWindow& window,
              const IGraphicsResourceManager& manager, float deltaTime) override;

    static std::unique_ptr<DeferredRenderer> create(
        IResourceManager* manager, const std::string& skyCubemapFile = std::string());

   private:
    // Geometry pass
    GeometryPass m_geometryPass;

    // Light pass
    LightPass m_lightPass;

    // Illumination pass
    IlluminationPass m_illuminationPass;

    // Post processing pass
    PostProcessPass m_postProcessPass;

    // Metering pass, it measures the brightness of the frame
    MeteringPass m_meteringPass;

    // Display pass for final screen draw
    DisplayPass m_displayPass;

    // Depth visualization pass
    DepthVisualizationPass m_depthVisualizationPass;

    // Fullscreen draw pass
};

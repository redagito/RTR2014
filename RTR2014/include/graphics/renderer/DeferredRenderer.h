#pragma once

#include <memory>

// Required by inheritance
#include "Renderer.h"

#include "DepthVisualizationPass.h"
#include "DisplayPass.h"
#include "GeometryPass.h"
#include "IlluminationPass.h"
#include "LightPass.h"
#include "PostProcessPass.h"

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
              const IGraphicsResourceManager& manager) override;

    static DeferredRenderer* create(IResourceManager* manager);

   private:
    // Geometry pass
    GeometryPass m_geometryPass;

    // Light pass
    LightPass m_lightPass;

    // Illumination pass
    IlluminationPass m_illuminationPass;

    // Post processing pass
    PostProcessPass m_postProcessPass;

    // Display pass for final screen draw
    DisplayPass m_displayPass;

    // Depth visualization pass
    DepthVisualizationPass m_depthVisualizationPass;

    // Fullscreen draw pass
};

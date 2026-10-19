#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "graphics/renderer/FrameBuffer.h"
#include "graphics/renderer/Transformer.h"
#include "resource/ResourceConfig.h"

class IGraphicsResourceManager;
class IResourceManager;
class IScene;
class ISceneQuery;
class ICamera;
class IWindow;
class Texture;

/**
* \brief Writes geometry data into the g-buffer.
*
* Fills the g-buffer with diffuse and glow, normal and specular, and depth
* attachments. The textures are sampled by the light and illumination passes.
*/
class GeometryPass
{
   public:
    GeometryPass();
    ~GeometryPass();

    /**
    * \brief Initializes the geometry pass resources.
    */
    bool init(IResourceManager* manager);

    /**
    * \brief Renders the visible scene objects into the g-buffer.
    */
    void render(const IScene& scene, ISceneQuery& query, const ICamera& camera,
                const IWindow& window, const IGraphicsResourceManager& manager);

    /**
    * \brief Returns the g-buffer depth texture.
    */
    const std::shared_ptr<Texture>& getDepthTexture() const;

    /**
    * \brief Returns the g-buffer texture storing base color and glow mask.
    */
    const std::shared_ptr<Texture>& getDiffuseGlowTexture() const;

    /**
    * \brief Returns the g-buffer texture storing normals and specularity.
    */
    const std::shared_ptr<Texture>& getNormalSpecularTexture() const;

   private:
    FrameBuffer m_geometryBuffer;
    std::shared_ptr<Texture> m_depthTexture = nullptr;
    std::shared_ptr<Texture> m_diffuseGlowTexture = nullptr;
    std::shared_ptr<Texture> m_normalSpecularTexture = nullptr;
    ResourceId m_geometryPassShaderId = -1;

    Transformer m_transformer;
};
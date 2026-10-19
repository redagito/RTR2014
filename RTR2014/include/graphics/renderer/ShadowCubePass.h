#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "graphics/renderer/FrameBuffer.h"
#include "resource/ResourceConfig.h"

class IGraphicsResourceManager;
class IResourceManager;
class IScene;
class ICamera;
class IWindow;
class Material;
class Mesh;
class ShaderProgram;
class Texture;

/**
* \brief Renders a shadow cube map from a point light position.
*
* Produces a cubemap of the scene as seen from the light, used for per-pixel
* shadow lookup when lighting the point light volume.
*/
class ShadowCubePass
{
   public:
    ShadowCubePass();
    ~ShadowCubePass();

    /**
    * \brief Initializes the shadow cube pass resources.
    */
    bool init(IResourceManager* manager);

    /**
    * \brief Renders the shadow cube map from the given camera position.
    */
    void render(const IScene& scene, const ICamera& camera, const IWindow& window,
                const IGraphicsResourceManager& manager);

    /**
    * \brief Returns the shadow cube map texture.
    */
    const std::shared_ptr<Texture>& getShadowMap() const;

   private:
    /**
    * \brief View direction and up vector for one cubemap face.
    */
    struct CameraDirection
    {
        GLenum cubemapFace;
        glm::vec3 target;
        glm::vec3 up;
    };

    static const CameraDirection cameraDirections[6];

    ResourceId m_shadowCubePassShaderId = -1;
    ShaderProgram* m_shadowCubePassShader = nullptr;
    FrameBuffer m_shadowCubeBuffer;
    std::shared_ptr<Texture> m_shadowCubeDepthTexture = nullptr;
    std::shared_ptr<Texture> m_shadowCubeTexture = nullptr;
};
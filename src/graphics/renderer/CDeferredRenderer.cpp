#include "CDeferredRenderer.h"

#include <cassert>
#include <string>

#include <glm/ext.hpp>

#include "graphics/IScene.h"
#include "graphics/ISceneQuery.h"
#include "graphics/ICamera.h"
#include "graphics/IWindow.h"

#include "resource/IResourceManager.h"

#include "graphics/IGraphicsResourceManager.h"
#include "graphics/resource/CMaterial.h"
#include "graphics/resource/CMesh.h"
#include "graphics/resource/CTexture.h"
#include "graphics/resource/CShaderProgram.h"
#include "graphics/renderer/CRenderBuffer.h"

#include "core/RendererCoreConfig.h"

#include "debug/RendererDebug.h"
#include "debug/Log.h"

CDeferredRenderer::CDeferredRenderer() { return; }

CDeferredRenderer::~CDeferredRenderer() { return; }

bool CDeferredRenderer::init(IResourceManager* manager)
{
    LOG_INFO("Initializing deferred renderer.");

    // Error check
    std::string error;
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
        return false;
    }

    // Init geometry pass resources
    if (!initGeometryPass(manager))
    {
        LOG_ERROR("Failed to intialize geometry pass.");
        return false;
    }

    // Shadow mapping
    // TODO Should be done in directional light pass
    if (!initShadowMapPass(manager))
    {
        LOG_ERROR("Failed to intialize shadow map pass.");
        return false;
    }

    if (!initShadowCubePass(manager))
    {
        LOG_ERROR("Failed to intialize shadow map pass.");
        return false;
    }

    // Init light pass resources
    if (!initPointLightPass(manager))
    {
        LOG_ERROR("Failed to initialize point light pass.");
        return false;
    }

    // Init directional light pass
    if (!initDirectionalLightPass(manager))
    {
        LOG_ERROR("Failed to initialize directional light pass.");
        return false;
    }

    // Init illumination pass
    if (!initIlluminationPass(manager))
    {
        LOG_ERROR("Failed to initialize illumination pass.");
        return false;
    }

    // Post process
    // TODO Dediacted post processor?
    if (!initPostProcessPass(manager))
    {
        LOG_ERROR("Failed to initialize post processing pass.");
        return false;
    }

    // Display pass
    if (!initDisplayPass(manager))
    {
        LOG_ERROR("Failed to initialize display pass.");
        return false;
    }

    if (!m_screenQuadPass.init(manager))
    {
        LOG_ERROR("Failed to initialize screen quad pass.");
        return false;
    }

    if (!initVisualizeDepthPass(manager))
    {
        LOG_ERROR("Failed to initialize depth visualization pass.");
    }
    return true;
}

void CDeferredRenderer::draw(const IScene& scene, const ICamera& camera, const IWindow& window,
                             const IGraphicsResourceManager& manager)
{
    // Draw init
    window.setActive();

    // Query visible scene objects and lights
    std::unique_ptr<ISceneQuery> query(std::move(scene.createQuery(camera)));

    // Geometry pass fills gbuffer
    geometryPass(scene, camera, window, manager, *query);

    // Light pass fills lbuffer
    lightPass(scene, camera, window, manager, *query);

    // Illumination pass renders lit scene from lbuffer and gbuffer
    illuminationPass(scene, camera, window, manager, *query);

    // Post processing pass
    postProcessPass(camera, window, manager, m_illuminationPassTexture);

    CFrameBuffer::setDefaultActive();
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    // Select rendering mode
    if (camera.getFeatureInfo().renderMode == RenderMode::Color)
    {
        displayPass(window, manager, m_diffuseGlowTexture);
    }
    else if (camera.getFeatureInfo().renderMode == RenderMode::Depth)
    {
        visualizeDepthPass(camera, window, manager);
    }
    else if (camera.getFeatureInfo().renderMode == RenderMode::Lights)
    {
        displayPass(window, manager, m_lightPassTexture);
    }
    else if (camera.getFeatureInfo().renderMode == RenderMode::Normals)
    {
        displayPass(window, manager, m_normalSpecularTexture);
    }
    else  // Final mode
    {
        // Final display pass
        displayPass(window, manager, m_postProcessPassOutputTexture);
    }

    // Post draw error check
    std::string error;
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
    }
}

CDeferredRenderer* CDeferredRenderer::create(IResourceManager* manager)
{
    CDeferredRenderer* renderer = new CDeferredRenderer;
    if (!renderer->init(manager))
    {
        delete renderer;
        renderer = nullptr;
        LOG_ERROR("Failed to create deferred renderer, initialization failed.");
    }
    return renderer;
}

void CDeferredRenderer::geometryPass(const IScene& scene, const ICamera& camera,
                                     const IWindow& window, const IGraphicsResourceManager& manager,
                                     ISceneQuery& query)
{
    // Set framebuffer
    m_geometryBuffer.setActive(GL_FRAMEBUFFER);
    // Clear buffer
    // TODO Should be retrieved as scene parameters
    // Diffuse, glow
    GLfloat diffuseGlow[] = {0.f, 0.f, 0.f, 0.f};
    GLfloat normalSpecular[] = {0.5f, 0.5f, 1.f, 0.f};
    glClearBufferfv(GL_COLOR, 0, diffuseGlow);
    // Normal, specular
    glClearBufferfv(GL_COLOR, 1, normalSpecular);

    // Clear
    glClear(GL_DEPTH_BUFFER_BIT);

    // Geometry pass, uses gbuffer fbo
    CShaderProgram* geometryPassShader = manager.getShaderProgram(m_geometryPassShaderId);

    // Depth
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Backface culling disabled for debugging
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Winding order, standard is counter-clockwise
    glFrontFace(GL_CCW);

    // Reset viewport
    glViewport(0, 0, window.getWidth(), window.getHeight());
    m_geometryBuffer.resize(window.getWidth(), window.getHeight());

    // Set view and projection matrices
    m_transformer.setViewMatrix(camera.getView());
    m_transformer.setProjectionMatrix(camera.getProjection());

    // Send view/projection to default shader
    geometryPassShader->setUniform(viewMatrixUniformName, m_transformer.getViewMatrix());
    geometryPassShader->setUniform(projectionMatrixUniformName,
                                   m_transformer.getProjectionMatrix());

    // Traverse visible objects
    while (query.hasNextObject())
    {
        // Get next visible object
        SceneObjectId id = query.getNextObject();

        // Object attributes
        ResourceId meshId = -1;
        ResourceId materialId = -1;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        // Retrieve object data
        if (!scene.getObject(id, meshId, materialId, position, rotation, scale))
        {
            // Invalid id
            LOG_ERROR("Invalid scene object id %l.", id);
        }
        else
        {
            // Resolve ids
            CMesh* mesh = manager.getMesh(meshId);
            CMaterial* material = manager.getMaterial(materialId);

            // Set transformations
            m_transformer.setPosition(position);
            m_transformer.setRotation(rotation);
            m_transformer.setScale(scale);

            if (material->hasCustomShader())
            {
                // Custom shaders not supported
                LOG_WARNING("Deferred renderer does not support custom material shaders.");
            }
            else
            {
                // Forward draw call
                draw(mesh, m_transformer.getTranslationMatrix(), m_transformer.getRotationMatrix(),
                     m_transformer.getScaleMatrix(), material, manager, geometryPassShader);
            }
        }
    }

    // Post draw error check
    std::string error;
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
    }

    // Disable geometry buffer
    m_geometryBuffer.setInactive(GL_FRAMEBUFFER);
}

// TODO extract in own file(s)
class StaticCamera : public ICamera
{
   public:
    StaticCamera(glm::mat4 view, glm::mat4 proj, glm::vec3 position)
    {
        m_view = view;
        m_proj = proj;
        m_pos = position;
    }

    virtual const glm::mat4& getView() const { return m_view; }

    virtual const glm::mat4& getProjection() const { return m_proj; }

    virtual glm::vec3 getPosition() const { return m_pos; }

   private:
    glm::mat4 m_view;
    glm::mat4 m_proj;
    glm::vec3 m_pos;
};

void CDeferredRenderer::shadowMapPass(const IScene& scene, const ICamera& camera,
                                      const IWindow& window,
                                      const IGraphicsResourceManager& manager)
{
    m_shadowMapPassShader = manager.getShaderProgram(m_shadowMapPassShaderId);

    // Set framebuffer
    m_shadowMapBuffer.setActive(GL_FRAMEBUFFER);

    // Clear
    glClear(GL_DEPTH_BUFFER_BIT);

    // Depth
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Backface culling disabled for debugging
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Winding order, standard is counter-clockwise
    glFrontFace(GL_CCW);

    // Reset viewport
    glViewport(0, 0, 4000, 4000);
    m_shadowMapBuffer.resize(4000, 4000);

    // Stores active transformations
    CTransformer transformer;

    // Set view and projection matrices
    transformer.setViewMatrix(camera.getView());
    transformer.setProjectionMatrix(camera.getProjection());

    // Query visible scene objects
    std::unique_ptr<ISceneQuery> query(std::move(scene.createQuery(camera)));

    // Send view/projection to default shader
    m_shadowMapPassShader->setUniform(viewMatrixUniformName, transformer.getViewMatrix());
    m_shadowMapPassShader->setUniform(projectionMatrixUniformName,
                                      transformer.getProjectionMatrix());

    // Traverse visible objects
    while (query->hasNextObject())
    {
        // Get next visible object
        SceneObjectId id = query->getNextObject();

        // Object attributes
        ResourceId meshId = -1;
        ResourceId materialId = -1;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        // Retrieve object data
        if (!scene.getObject(id, meshId, materialId, position, rotation, scale))
        {
            // Invalid id
            LOG_ERROR("Invalid scene object id %l.", id);
        }
        else
        {
            // Resolve ids
            CMesh* mesh = manager.getMesh(meshId);
            CMaterial* material = manager.getMaterial(materialId);

            // Set transformations
            transformer.setPosition(position);
            transformer.setRotation(rotation);
            transformer.setScale(scale);

            if (material->hasCustomShader())
            {
                // Custom shaders not supported
                LOG_WARNING("Deferred renderer does not support custom material shaders.");
            }
            else
            {
                // Forward draw call
                draw(mesh, transformer.getTranslationMatrix(), transformer.getRotationMatrix(),
                     transformer.getScaleMatrix(), material, manager, m_shadowMapPassShader);
            }
        }
    }

    // Post draw error check
    std::string error;
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
    }

    // Disable geometry buffer
    m_shadowMapBuffer.setInactive(GL_FRAMEBUFFER);

    glCullFace(GL_BACK);
}

struct CameraDirection
{
    GLenum cubemapFace;
    glm::vec3 target;
    glm::vec3 up;
};

CameraDirection g_cameraDirections[6] = {
    {GL_TEXTURE_CUBE_MAP_POSITIVE_X, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
    {GL_TEXTURE_CUBE_MAP_NEGATIVE_X, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
    {GL_TEXTURE_CUBE_MAP_POSITIVE_Y, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
    {GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
    {GL_TEXTURE_CUBE_MAP_POSITIVE_Z, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
    {GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)}};

void CDeferredRenderer::shadowCubePass(const IScene& scene, const ICamera& camera,
                                       const IWindow& window,
                                       const IGraphicsResourceManager& manager)
{
    m_shadowCubePassShader = manager.getShaderProgram(m_shadowCubePassShaderId);
    m_shadowCubePassShader->setActive();

    // Depth
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Backface culling disabled for debugging
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glDisable(GL_BLEND);

    // Winding order, standard is counter-clockwise
    glFrontFace(GL_CCW);

    // Reset viewport
    glViewport(0, 0, 1024, 1024);
    // m_shadowMapBuffer.resize(1024, 1024);

    m_shadowCubePassShader->setUniform(lightPositionUniformName, camera.getPosition());

    glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);

    for (unsigned int i = 0; i < 6; ++i)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_shadowCubeBuffer.getId());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               g_cameraDirections[i].cubemapFace, m_shadowCubeTexture->getId(), 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glViewport(0, 0, 1024, 1024);

        // Send view/projection to default shader
        glm::mat4 view =
            glm::lookAt(camera.getPosition(), camera.getPosition() + g_cameraDirections[i].target,
                        g_cameraDirections[i].up);

        m_shadowCubePassShader->setUniform(projectionMatrixUniformName, camera.getProjection());
        m_shadowCubePassShader->setUniform(viewMatrixUniformName, view);

        // Query visible scene objects
        std::unique_ptr<ISceneQuery> query(std::move(scene.createQuery(camera)));

        // Traverse visible objects
        while (query->hasNextObject())
        {
            // Get next visible object
            SceneObjectId id = query->getNextObject();

            // Object attributes
            ResourceId meshId = -1;
            ResourceId materialId = -1;
            glm::vec3 position;
            glm::vec3 rotation;
            glm::vec3 scale;

            // Retrieve object data
            if (!scene.getObject(id, meshId, materialId, position, rotation, scale))
            {
                // Invalid id
                LOG_ERROR("Invalid scene object id %l.", id);
            }
            else
            {
                // Resolve ids
                CMesh* mesh = manager.getMesh(meshId);
                CMaterial* material = manager.getMaterial(materialId);

                CTransformer transformer;

                // Set transformations
                transformer.setPosition(position);
                transformer.setRotation(rotation);
                transformer.setScale(scale);

                // Forward draw call
                draw(mesh, transformer.getTranslationMatrix(), transformer.getRotationMatrix(),
                     transformer.getScaleMatrix(), material, manager, m_shadowCubePassShader);
            }
        }
    }

    // Post draw error check
    std::string error;
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
    }

    // Disable buffer
    m_shadowCubeBuffer.setInactive(GL_FRAMEBUFFER);

    glClearColor(0, 0, 0, 0);
    glCullFace(GL_BACK);
}

void CDeferredRenderer::lightPass(const IScene& scene, const ICamera& camera, const IWindow& window,
                                  const IGraphicsResourceManager& manager, ISceneQuery& query)
{
    // Prepare light pass frame buffer
    glViewport(0, 0, window.getWidth(), window.getHeight());
    // Resize
    m_lightPassFrameBuffer.resize(window.getWidth(), window.getHeight());
    // Enable light buffer
    m_lightPassFrameBuffer.setActive(GL_FRAMEBUFFER);

    // Retrieve ambient light
    glm::vec3 ambientColor;
    float ambientIntensity;
    scene.getAmbientLight(ambientColor, ambientIntensity);
    // Initialize light buffer with ambient light
    glClearColor(ambientColor.x * ambientIntensity, ambientColor.y * ambientIntensity,
                 ambientColor.z * ambientIntensity, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // No depth testing for light volumes
    glDisable(GL_DEPTH_TEST);
    // Additive blending for light accumulation
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    // Draw point light volumes
    pointLightPass(scene, camera, window, manager, query);

    // Draw directional lights
    directionalLightPass(scene, camera, window, manager, query);

    // Reset state and cleanup
    glDisable(GL_BLEND);
    m_lightPassFrameBuffer.setInactive(GL_FRAMEBUFFER);
}

void CDeferredRenderer::pointLightPass(const IScene& scene, const ICamera& camera,
                                       const IWindow& window,
                                       const IGraphicsResourceManager& manager, ISceneQuery& query)
{
    // Point light pass
    CShaderProgram* pointLightPassShader = manager.getShaderProgram(m_pointLightPassShaderId);
    if (pointLightPassShader == nullptr)
    {
        LOG_ERROR("Shader program for point light pass could not be retrieved.");
        return;
    }

    CMesh* pointLightMesh = manager.getMesh(m_pointLightSphereId);
    if (pointLightMesh == nullptr)
    {
        LOG_ERROR("Mesh object for point light pass could not be retrieved.");
        return;
    }

    // Render point light volumes into light buffer
    while (query.hasNextPointLight())
    {
        // Retrieve light id
        SceneObjectId pointLightId = query.getNextPointLight();
        // Retrieve light parameters
        glm::vec3 position;
        glm::vec3 color;
        float intensity;
        float radius;
        bool castsShadow;

        if (!scene.getPointLight(pointLightId, position, radius, color, intensity, castsShadow))
        {
            LOG_ERROR("Failed to retrieve point light data from point light id %i.", pointLightId);
        }
        else
        {
            // fov should be 90.f instead of 89.54f, but this does not work, because the view is too
            // wide in this case. 89.54f is determined by testing. 89.53 is already too small and
            // 89.55 too big.
            glm::mat4 shadowProj = glm::perspective(89.54f, 1.0f, 0.01f, radius * 1.5f);
            StaticCamera shadowCamera = StaticCamera(glm::mat4(), shadowProj, position);
            shadowCubePass(scene, shadowCamera, window, manager);

            // Prepare light pass frame buffer
            glViewport(0, 0, window.getWidth(), window.getHeight());
            m_lightPassFrameBuffer.setActive(GL_FRAMEBUFFER);

            // No depth testing for light volumes
            glDisable(GL_DEPTH_TEST);
            // Additive blending for light accumulation
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);

            // Cull front facing faces
            glCullFace(GL_FRONT);

            // Set textures for point light pass
            // Set depth texture
            m_depthTexture->setActive(lightPassDepthTextureUnit);
            pointLightPassShader->setUniform(depthTextureUniformName, lightPassDepthTextureUnit);

            // Set texture with world space normal and specular power
            m_normalSpecularTexture->setActive(lightPassNormalSpecularTextureUnit);
            pointLightPassShader->setUniform(normalSpecularTextureUniformName,
                                             lightPassNormalSpecularTextureUnit);

            // Set shadow texture for shadow mapping
            glActiveTexture(GL_TEXTURE0 + lightPassShadowMapTextureUnit);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowCubeTexture->getId());
            pointLightPassShader->setUniform(shadowCubeTextureUniformName,
                                             lightPassShadowMapTextureUnit);

            // Set screen size
            pointLightPassShader->setUniform(screenWidthUniformName, (float)window.getWidth());
            pointLightPassShader->setUniform(screenHeightUniformName, (float)window.getHeight());

			// Camera position
			pointLightPassShader->setUniform(cameraPositionUniformName, camera.getPosition());

            // Inverse view-projection
            pointLightPassShader->setUniform(inverseViewProjectionMatrixUniformName,
                                             m_transformer.getInverseViewProjectionMatrix());

            m_transformer.setPosition(position);
            // Scale is calculated from light radius
            // Sphere model has radius 1.f
            m_transformer.setScale(glm::vec3(radius));
            // Point lights do not have rotation
            m_transformer.setRotation(glm::vec3(0.f));

            // Light volume transformation for vertex shader
            pointLightPassShader->setUniform(modelViewProjectionMatrixUniformName,
                                             m_transformer.getModelViewProjectionMatrix());

            // Set point light parameters for fragment shader
            pointLightPassShader->setUniform(lightPositionUniformName, position);
            pointLightPassShader->setUniform(lightRadiusUniformName, radius);
            pointLightPassShader->setUniform(lightColorUniformName, color);
            pointLightPassShader->setUniform(lightIntensityUniformName, intensity);
            ARenderer::draw(pointLightMesh);
        }
    }

    return;
}

void CDeferredRenderer::directionalLightPass(const IScene& scene, const ICamera& camera,
                                             const IWindow& window,
                                             const IGraphicsResourceManager& manager,
                                             ISceneQuery& query)
{
    // Restrieve shader
    CShaderProgram* directionalLightPassShader =
        manager.getShaderProgram(m_directionalLightPassShaderId);
    if (directionalLightPassShader == nullptr)
    {
        LOG_ERROR("Shader program for directional light pass could not be retrieved.");
        return;
    }

    // Retrieve fullscreen quad mesh
    CMesh* quadMesh = manager.getMesh(m_directionalLightScreenQuadId);
    if (quadMesh == nullptr)
    {
        LOG_ERROR("Mesh object for directional light pass could not be retrieved.");
        return;
    }

    // Render point light volumes into light buffer
    while (query.hasNextDirectionalLight())
    {
        // Retrieve light id
        SceneObjectId directionalLightId = query.getNextDirectionalLight();
        // Retrieve light parameters
        glm::vec3 direction;
        glm::vec3 color;
        float intensity;
        bool castsShadow;

        if (!scene.getDirectionalLight(directionalLightId, direction, color, intensity,
                                       castsShadow))
        {
            LOG_ERROR("Failed to retrieve directional light data from point light id %i.",
                      directionalLightId);
        }
        else
        {
            // Create shadow camera
            glm::mat4 shadowView =
                glm::lookAt(glm::vec3(0), glm::normalize(direction), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 shadowProj = glm::ortho(-150.0f, 150.0f, -150.0f, 150.0f, -150.0f, 150.0f);
            StaticCamera shadowCamera = StaticCamera(shadowView, shadowProj, camera.getPosition());

            // Render shadow map
            shadowMapPass(scene, shadowCamera, window, manager);

            // Prepare light pass frame buffer
            m_lightPassFrameBuffer.setActive(GL_FRAMEBUFFER);
            glViewport(0, 0, window.getWidth(), window.getHeight());

            // No depth testing for light volumes
            glDisable(GL_DEPTH_TEST);
            // Additive blending for light accumulation
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);

            // Reset culling
            glCullFace(GL_BACK);

            // Set shader active
            directionalLightPassShader->setActive();

            // Set textures for point light pass
            // Set depth texture
            m_depthTexture->setActive(lightPassDepthTextureUnit);
            directionalLightPassShader->setUniform(depthTextureUniformName,
                                                   lightPassDepthTextureUnit);

            // Set texture with world space normal and specular power
            m_normalSpecularTexture->setActive(lightPassNormalSpecularTextureUnit);
            directionalLightPassShader->setUniform(normalSpecularTextureUniformName,
                                                   lightPassNormalSpecularTextureUnit);

            // Set shadow texture for shadow mapping
            m_shadowDepthTexture->setActive(lightPassShadowMapTextureUnit);
            directionalLightPassShader->setUniform(shadowMapTextureUniformName,
                                                   lightPassShadowMapTextureUnit);

            // Set screen size
            directionalLightPassShader->setUniform(screenWidthUniformName,
                                                   (float)window.getWidth());
            directionalLightPassShader->setUniform(screenHeightUniformName,
                                                   (float)window.getHeight());

            // Inverse view-projection
            directionalLightPassShader->setUniform(inverseViewProjectionMatrixUniformName,
                                                   m_transformer.getInverseViewProjectionMatrix());

            // Shadow ViewProjectionBias
            glm::mat4 shadowViewProjBiasMatrix =
                glm::mat4(0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f,
                          0.5f, 0.5f, 0.5f, 1.0f) *
                shadowProj * shadowView;
            directionalLightPassShader->setUniform(shadowViewProjectionBiasMatrixUniformName,
                                                   shadowViewProjBiasMatrix);

            // Set directional light parameters
            directionalLightPassShader->setUniform(lightDirectionUniformName, direction);
            directionalLightPassShader->setUniform(lightColorUniformName, color);
            directionalLightPassShader->setUniform(lightIntensityUniformName, intensity);
            ARenderer::draw(quadMesh);
        }
    }

    // Reset culling
    glCullFace(GL_BACK);

    return;
}

void CDeferredRenderer::illuminationPass(const IScene& scene, const ICamera& camera,
                                         const IWindow& window,
                                         const IGraphicsResourceManager& manager,
                                         ISceneQuery& query)
{
    // Reset viewport
    glViewport(0, 0, window.getWidth(), window.getHeight());
    m_illumationPassFrameBuffer.resize(window.getWidth(), window.getHeight());
    // TODO Clear illumination pass buffer?

    // Get illumination shader
    CShaderProgram* illuminationShader = manager.getShaderProgram(m_illuminationPassShaderId);
    if (illuminationShader == nullptr)
    {
        LOG_ERROR("Shader program for illumination pass could not be retrieved.");
        return;
    }

    // Get screen space quad
    CMesh* quadMesh = manager.getMesh(m_illuminationPassScreenQuadId);
    if (quadMesh == nullptr)
    {
        LOG_ERROR("Mesh object for illumination pass could not be retrieved.");
        return;
    }

    // Set textures
    // L-buffer light texture
    m_lightPassTexture->setActive(illuminationPassLightTextureUnit);
    illuminationShader->setUniform(lightTextureUniformName, illuminationPassLightTextureUnit);

    // G-buffer diffuse glow texture
    m_diffuseGlowTexture->setActive(illuminationPassDiffuseGlowTextureUnit);
    illuminationShader->setUniform(diffuseGlowTextureUniformName,
                                   illuminationPassDiffuseGlowTextureUnit);

    // G-buffer depth texture
    m_depthTexture->setActive(illuminationPassDepthTextureUnit);
    illuminationShader->setUniform(depthTextureUniformName, illuminationPassDepthTextureUnit);

    // Screen parameters
    illuminationShader->setUniform(screenWidthUniformName, (float)window.getWidth());
    illuminationShader->setUniform(screenHeightUniformName, (float)window.getHeight());

    // Draw into frame buffer
    m_illumationPassFrameBuffer.setActive(GL_FRAMEBUFFER);
    ARenderer::draw(quadMesh);
    m_illumationPassFrameBuffer.setInactive(GL_FRAMEBUFFER);
}

void CDeferredRenderer::postProcessPass(const ICamera& camera, const IWindow& window,
                                        const IGraphicsResourceManager& manager,
                                        const std::shared_ptr<CTexture>& texture)
{
    // Reset viewport
    glViewport(0, 0, window.getWidth(), window.getHeight());
    // Resize frame buffer
    m_postProcessPassFrameBuffer0.resize(window.getWidth(), window.getHeight());
    m_postProcessPassFrameBuffer1.resize(window.getWidth(), window.getHeight());
    m_postProcessPassFrameBuffer2.resize(window.getWidth(), window.getHeight());

    // Pass 1: fxaa
    m_postProcessPassFrameBuffer0.setActive(GL_FRAMEBUFFER);
    if (camera.getFeatureInfo().fxaaActive)
    {
        fxaaPass(window, manager, texture);
    }
    else
    {
        // Passthrough
        passthroughPass(window, manager, texture);
    }

    // Pass 2: fog
    // TODO Fog parameter
    m_postProcessPassFrameBuffer1.setActive(GL_FRAMEBUFFER);
    fogPass(camera, window, manager, m_postProcessPassTexture0);
    // Foggy scene in tex1

    // Pass 3: dof
	m_postProcessPassFrameBuffer0.setActive(GL_FRAMEBUFFER);
    if (camera.getFeatureInfo().dofActive)
    {
        // Pass 3.1: gauss blur
        gaussBlurVerticalPass(window, manager, m_postProcessPassTexture1);
        // Blurred in tex0

        m_postProcessPassFrameBuffer2.setActive(GL_FRAMEBUFFER);
        gaussBlurHorizontalPass(window, manager, m_postProcessPassTexture0);
        // Blurred in tex 2

        // TODO DOF parameter
        m_postProcessPassFrameBuffer0.setActive(GL_FRAMEBUFFER);
        depthOfFieldPass(camera, window, manager, m_postProcessPassTexture1,
                         m_postProcessPassTexture2);
    }
    else
    {
        // Passthrough
        passthroughPass(window, manager, texture);
    }
    // Processed scene in tex 0

	if (camera.getFeatureInfo().godRayActive || camera.getFeatureInfo().renderMode == RenderMode::GodRay)
	{
		// TODO God ray pass disabled, not working as intended
		// God ray pass 1
		m_postProcessPassFrameBuffer1.setActive(GL_FRAMEBUFFER);
		godRayPass1(window, manager, m_postProcessPassTexture0);
		// God ray texture in tex 1

		// Godray debug mode
		if (camera.getFeatureInfo().renderMode == RenderMode::GodRay)
		{
			// Return generated godray texture
			m_postProcessPassOutputTexture = m_postProcessPassTexture1;
			return;
		}

		// God ray pass 2
		m_postProcessPassFrameBuffer2.setActive(GL_FRAMEBUFFER);
		godRayPass2(window, manager, m_postProcessPassTexture0, m_postProcessPassTexture1);
	}
	else
	{
		// Passthrough into tex 2
		m_postProcessPassFrameBuffer2.setActive(GL_FRAMEBUFFER);
		passthroughPass(window, manager, texture);
	}

    // Set output texture
    m_postProcessPassOutputTexture = m_postProcessPassTexture2;
	return;
}

void CDeferredRenderer::fxaaPass(const IWindow& window, const IGraphicsResourceManager& manager,
                                 const std::shared_ptr<CTexture>& texture)
{
    // Get fxaa shader
    CShaderProgram* fxaaShader = manager.getShaderProgram(m_fxaaPassShaderId);
    if (fxaaShader == nullptr)
    {
        LOG_ERROR("Shader program for fxaa pass could not be retrieved.");
        return;
    }

    // Get screen space quad
    CMesh* quadMesh = manager.getMesh(m_postProcessScreenQuadId);
    if (quadMesh == nullptr)
    {
        LOG_ERROR("Mesh object for fxaa pass could not be retrieved.");
        return;
    }

    // Set input texture
    texture->setActive(fxaaPassInputTextureUnit);
    fxaaShader->setUniform(sceneTextureUniformName, fxaaPassInputTextureUnit);

    // Set screen size
    fxaaShader->setUniform(screenWidthUniformName, (float)window.getWidth());
    fxaaShader->setUniform(screenHeightUniformName, (float)window.getHeight());

    ARenderer::draw(quadMesh);
}

void CDeferredRenderer::fogPass(const ICamera& camera, const IWindow& window,
                                const IGraphicsResourceManager& manager,
                                const std::shared_ptr<CTexture>& texture)
{
    // Get fog shader
    CShaderProgram* fogShader = manager.getShaderProgram(m_fogPassShaderId);
    if (fogShader == nullptr)
    {
        LOG_ERROR("Shader program for fog pass could not be retrieved.");
        return;
    }

    // Get screen space quad
    CMesh* quadMesh = manager.getMesh(m_postProcessScreenQuadId);
    if (quadMesh == nullptr)
    {
        LOG_ERROR("Mesh object for fog pass could not be retrieved.");
        return;
    }

    fogShader->setActive();

    texture->setActive(fogPassSceneTextureUnit);
    fogShader->setUniform(sceneTextureUniformName, fogPassSceneTextureUnit);

    m_depthTexture->setActive(fogPassDepthTextureUnit);
    fogShader->setUniform(depthTextureUniformName, fogPassDepthTextureUnit);

    fogShader->setUniform(screenWidthUniformName, (float)window.getWidth());
    fogShader->setUniform(screenHeightUniformName, (float)window.getHeight());

    int fogType = 0;
    switch (camera.getFeatureInfo().fogType)
    {
    case FogType::None:
        fogType = 0;
        break;
    case FogType::Linear:
        fogType = 1;
        break;
    case FogType::Exp:
        fogType = 2;
        break;
    case FogType::Exp2:
        fogType = 3;
        break;
    default:
        fogType = 0;
    }
    fogShader->setUniform(fogPassTypeUniformName, fogType);

    ARenderer::draw(quadMesh);
}

void CDeferredRenderer::gaussBlurVerticalPass(const IWindow& window,
                                              const IGraphicsResourceManager& manager,
                                              const std::shared_ptr<CTexture>& texture)
{
    // Get gauss shader
    CShaderProgram* shader = manager.getShaderProgram(m_gaussBlurVerticalShaderId);
    if (shader == nullptr)
    {
        LOG_ERROR("Shader program for vertical gauss blur pass could not be retrieved.");
        return;
    }

    // Get screen space quad
    CMesh* quadMesh = manager.getMesh(m_postProcessScreenQuadId);
    if (quadMesh == nullptr)
    {
        LOG_ERROR("Mesh object for vertical gauss blur pass could not be retrieved.");
        return;
    }

    // Input texture
    texture->setActive(gaussBlurVerticalPassInputTextureUnit);
    shader->setUniform(sceneTextureUniformName, gaussBlurVerticalPassInputTextureUnit);

    // Blur parameter
    shader->setUniform(blurStrengthUniformName, 3.f);

    /// Screen size
    shader->setUniform(screenWidthUniformName, (float)window.getWidth());
    shader->setUniform(screenHeightUniformName, (float)window.getHeight());

    // Perform pass
    ARenderer::draw(quadMesh);
}

void CDeferredRenderer::gaussBlurHorizontalPass(const IWindow& window,
                                                const IGraphicsResourceManager& manager,
                                                const std::shared_ptr<CTexture>& texture)
{
    // Get gauss shader
    CShaderProgram* shader = manager.getShaderProgram(m_gaussBlurHorizontalShaderId);
    if (shader == nullptr)
    {
        LOG_ERROR("Shader program for horizontal gauss blur pass could not be retrieved.");
        return;
    }

    // Get screen space quad
    CMesh* quadMesh = manager.getMesh(m_postProcessScreenQuadId);
    if (quadMesh == nullptr)
    {
        LOG_ERROR("Mesh object for horizontal gauss blur pass could not be retrieved.");
        return;
    }

    // Input texture
    texture->setActive(gaussBlurHoriontalPassInputTextureUnit);
    shader->setUniform(sceneTextureUniformName, gaussBlurHoriontalPassInputTextureUnit);

    // Blur parameter
    shader->setUniform(blurStrengthUniformName, 3.f);

    /// Screen size
    shader->setUniform(screenWidthUniformName, (float)window.getWidth());
    shader->setUniform(screenHeightUniformName, (float)window.getHeight());

    // Perform pass
    ARenderer::draw(quadMesh);
}

void CDeferredRenderer::depthOfFieldPass(const ICamera& camera, const IWindow& window,
                                         const IGraphicsResourceManager& manager,
                                         const std::shared_ptr<CTexture>& sceneTexture,
                                         const std::shared_ptr<CTexture>& blurTexture)
{
    // Get shader
    CShaderProgram* shader = manager.getShaderProgram(m_depthOfFieldPassShaderId);
    if (shader == nullptr)
    {
        LOG_ERROR("Shader program for depth-of-field pass could not be retrieved.");
        return;
    }

    // Get screen space quad
    CMesh* quadMesh = manager.getMesh(m_postProcessScreenQuadId);
    if (quadMesh == nullptr)
    {
        LOG_ERROR("Mesh object for depth-of-field pass could not be retrieved.");
        return;
    }

    // Input scene texture
    sceneTexture->setActive(depthOfFieldPassSceneTextureUnit);
    shader->setUniform(sceneTextureUniformName, depthOfFieldPassSceneTextureUnit);

    // Input blur texture
    blurTexture->setActive(depthOfFieldPassBlurTextureUnit);
    shader->setUniform(blurTextureUniformName, depthOfFieldPassBlurTextureUnit);

    // Input depth texture
    m_depthTexture->setActive(depthOfFieldPassDepthTextureUnit);
    shader->setUniform(depthTextureUniformName, depthOfFieldPassDepthTextureUnit);

    // Depth-of-field parameters
    shader->setUniform(blurNearUniformName, camera.getFeatureInfo().dofNearBlur);
    shader->setUniform(focusNearUniformName, camera.getFeatureInfo().dofNearFocus);
    shader->setUniform(focusFarUniformName, camera.getFeatureInfo().dofFarFocus);
    shader->setUniform(blurFarUniformName, camera.getFeatureInfo().dofFarBlur);

    // Camera position
    shader->setUniform(cameraPositionUniformName, camera.getPosition());

    // Transformation
    shader->setUniform(inverseViewProjectionMatrixUniformName,
                       m_transformer.getInverseViewProjectionMatrix());

    /// Screen size
    shader->setUniform(screenWidthUniformName, (float)window.getWidth());
    shader->setUniform(screenHeightUniformName, (float)window.getHeight());

    // Perform pass
    ARenderer::draw(quadMesh);
}

void CDeferredRenderer::displayPass(const IWindow& window, const IGraphicsResourceManager& manager,
                                    const std::shared_ptr<CTexture>& texture)
{
    // Set main FBO active
    CFrameBuffer::setDefaultActive();
    passthroughPass(window, manager, texture);
}

void CDeferredRenderer::passthroughPass(const IWindow& window,
                                        const IGraphicsResourceManager& manager,
                                        const std::shared_ptr<CTexture>& texture)
{
    // Reset viewport
    glViewport(0, 0, window.getWidth(), window.getHeight());

    // Get display shader
    CShaderProgram* displayShader = manager.getShaderProgram(m_displayPassShaderId);
    if (displayShader == nullptr)
    {
        LOG_ERROR("Shader program for display pass could not be retrieved.");
        return;
    }

    // Get screen space quad
    CMesh* quadMesh = manager.getMesh(m_illuminationPassScreenQuadId);
    if (quadMesh == nullptr)
    {
        LOG_ERROR("Mesh object for illumination pass could not be retrieved.");
        return;
    }

    // Scene texture
    texture->setActive(displayPassSceneTextureUnit);
    displayShader->setUniform(sceneTextureUniformName, displayPassSceneTextureUnit);

    // Screen parameters
    displayShader->setUniform(screenWidthUniformName, (float)window.getWidth());
    displayShader->setUniform(screenHeightUniformName, (float)window.getHeight());

    ARenderer::draw(quadMesh);
}

void CDeferredRenderer::godRayPass1(const IWindow& window, const IGraphicsResourceManager& manager,
                                    const std::shared_ptr<CTexture>& texture)
{
    // Get shader
    CShaderProgram* shader = manager.getShaderProgram(m_godRayPass1ShaderId);
    if (shader == nullptr)
    {
        LOG_ERROR("Shader program for god ray 1 pass could not be retrieved.");
        return;
    }

    // Get screen space quad
    CMesh* quadMesh = manager.getMesh(m_postProcessScreenQuadId);
    if (quadMesh == nullptr)
    {
        LOG_ERROR("Mesh object for god ray 1 pass could not be retrieved.");
        return;
    }

    // Input texture
    texture->setActive(godRayPass1InputTextureUnit);
    shader->setUniform(sceneTextureUniformName, godRayPass1InputTextureUnit);

    // Depth texture
    m_depthTexture->setActive(godRayPass1DepthTextureUnit);
    shader->setUniform(depthTextureUniformName, godRayPass1DepthTextureUnit);

    // Inverse projection
    shader->setUniform(inverseProjectionMatrixUniformName,
                       m_transformer.getInverseProjectionMatrix());

    // Light position
    shader->setUniform(lightPositionScreenUniformName, glm::vec2(0.5, 0.5));

    /// Screen size
    shader->setUniform(screenWidthUniformName, (float)window.getWidth());
    shader->setUniform(screenHeightUniformName, (float)window.getHeight());

    // Perform pass
    ARenderer::draw(quadMesh);
}

void CDeferredRenderer::godRayPass2(const IWindow& window, const IGraphicsResourceManager& manager,
                                    const std::shared_ptr<CTexture>& sceneTexture,
                                    const std::shared_ptr<CTexture>& godrayTexture)
{
    // Get gauss shader
    CShaderProgram* shader = manager.getShaderProgram(m_godRayPass2ShaderId);
    if (shader == nullptr)
    {
        LOG_ERROR("Shader program for god ray 2 pass could not be retrieved.");
        return;
    }

    // Get screen space quad
    CMesh* quadMesh = manager.getMesh(m_postProcessScreenQuadId);
    if (quadMesh == nullptr)
    {
        LOG_ERROR("Mesh object for god ray 2 pass could not be retrieved.");
        return;
    }

    // Scene texture
    sceneTexture->setActive(godRayPass2SceneTextureUnit);
    shader->setUniform(sceneTextureUniformName, godRayPass2SceneTextureUnit);

    // God ray texture
    godrayTexture->setActive(godRayPass2GodRayTextureUnit);
    shader->setUniform(godRayTextureUniformName, godRayPass2GodRayTextureUnit);

    /// Screen size
    shader->setUniform(screenWidthUniformName, (float)window.getWidth());
    shader->setUniform(screenHeightUniformName, (float)window.getHeight());

    // Perform pass
    ARenderer::draw(quadMesh);
}

void CDeferredRenderer::visualizeDepthPass(const ICamera& camera, const IWindow& window,
                                           const IGraphicsResourceManager& manager)
{
    // Get shader
    CShaderProgram* shader = manager.getShaderProgram(m_visualizeDepthPassShaderId);
    if (shader == nullptr)
    {
        LOG_ERROR("Shader program for depth visualization pass could not be retrieved.");
        return;
    }

    // Get screen space quad
    CMesh* quadMesh = manager.getMesh(m_postProcessScreenQuadId);
    if (quadMesh == nullptr)
    {
        LOG_ERROR("Mesh object for depth visualization pass could not be retrieved.");
        return;
    }

    // Depth texture
    m_depthTexture->setActive(visualizeDepthPassDepthTextureUnit);
    shader->setUniform(depthTextureUniformName, visualizeDepthPassDepthTextureUnit);

    /// Screen size
    shader->setUniform(screenWidthUniformName, (float)window.getWidth());
    shader->setUniform(screenHeightUniformName, (float)window.getHeight());

    // camera parameters
    // TODO Replace
    shader->setUniform(cameraZNearUniformName, 0.01f);
    shader->setUniform(cameraZFarUniformName, 1000.f);

    // Perform pass
    ARenderer::draw(quadMesh);
}

void CDeferredRenderer::draw(CMesh* mesh, const glm::mat4& translation, const glm::mat4& rotation,
                             const glm::mat4& scale, CMaterial* material,
                             const IGraphicsResourceManager& manager, CShaderProgram* shader)
{
    std::string error;
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
    }

    shader->setActive();

    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
    }

    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
    }

    // Transformation matrices
    shader->setUniform(translationMatrixUniformName, translation);
    shader->setUniform(rotationMatrixUniformName, rotation);
    shader->setUniform(scaleMatrixUniformName, scale);
    shader->setUniform(modelMatrixUniformName, translation * rotation * scale);

    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
    }

    // Send material textures to shader
    if (material->hasDiffuse())
    {
        material->getDiffuse()->setActive(diffuseTextureUnit);
    }
    else
    {
        manager.getDefaultDiffuseTexture()->setActive(diffuseTextureUnit);
    }
    shader->setUniform(diffuseTextureUniformName, diffuseTextureUnit);

    if (material->hasNormal())
    {
        material->getNormal()->setActive(normalTextureUnit);
    }
    else
    {
        // Use default texture
        manager.getDefaultNormalTexture()->setActive(normalTextureUnit);
    }
    shader->setUniform(normalTextureUniformName, normalTextureUnit);

    if (material->hasSpecular())
    {
        material->getSpecular()->setActive(specularTextureUnit);
    }
    else
    {
        // Use default texture
        manager.getDefaultSpecularTexture()->setActive(specularTextureUnit);
    }
    shader->setUniform(specularTextureUniformName, specularTextureUnit);

    if (material->hasGlow())
    {
        material->getGlow()->setActive(glowTextureUnit);
    }
    else
    {
        // Use default texture
        manager.getDefaultGlowTexture()->setActive(glowTextureUnit);
    }
    shader->setUniform(glowTextureUniformName, glowTextureUnit);

    // TODO Sort materials with alpha texture for blending.
    if (material->hasAlpha())
    {
        material->getAlpha()->setActive(alphaTextureUnit);
    }
    else
    {
        // Use default texture
        manager.getDefaultAlphaTexture()->setActive(alphaTextureUnit);
    }
    shader->setUniform(alphaTextureUniformName, alphaTextureUnit);

    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
    }

    // Draw mesh
    // TODO Consider custom shader bindings for meshes
    ARenderer::draw(mesh);

    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
    }
    // TODO Cleanup?
}

bool CDeferredRenderer::initGeometryPass(IResourceManager* manager)
{
    // Init geometry pass shader
    // TODO Read file name from config?
    // Geometry pass shader for filling gbuffer
    std::string geometryPassShaderFile("data/shader/deferred/geometry_pass.ini");
    m_geometryPassShaderId = manager->loadShader(geometryPassShaderFile);

    // Check if ok
    if (m_geometryPassShaderId == invalidResource)
    {
        LOG_ERROR("Failed to initialize the shader from file %s.", geometryPassShaderFile.c_str());
        return false;
    }

    // Init gbuffer
    // Diffuse texture, stores base color and glow mask.
    m_diffuseGlowTexture = std::make_shared<CTexture>();
    m_diffuseGlowTexture->init(800, 600, GL_RGBA);

    // Normal texture, store x and y, z normals and specularity.
    m_normalSpecularTexture = std::make_shared<CTexture>();
    m_normalSpecularTexture->init(800, 600, GL_RGBA16F);

    // Depth texture
    m_depthTexture = std::make_shared<CTexture>();
    m_depthTexture->init(800, 600, GL_DEPTH_COMPONENT24);

    // Error check
    std::string error;
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
        return false;
    }

    // Total 96 bit per pixel
    m_geometryBuffer.attach(m_depthTexture, GL_DEPTH_ATTACHMENT);
    m_geometryBuffer.attach(m_diffuseGlowTexture, GL_COLOR_ATTACHMENT0);
    m_geometryBuffer.attach(m_normalSpecularTexture, GL_COLOR_ATTACHMENT1);

    // Error check
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
        return false;
    }

    LOG_INFO("GBuffer state: %s.", m_geometryBuffer.getState().c_str());

    // Reset framebuffer
    m_geometryBuffer.setInactive(GL_FRAMEBUFFER);

    // Error check
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
        return false;
    }
    return true;
}

bool CDeferredRenderer::initShadowCubePass(IResourceManager* manager)
{
    std::string shaderFile("data/shader/shadow_cube_pass.ini");
    m_shadowCubePassShaderId = manager->loadShader(shaderFile);

    if (m_shadowCubePassShaderId == invalidResource)
    {
        LOG_ERROR("Failed to initialize the shader from file %s.", shaderFile.c_str());
        return false;
    }

    m_shadowCubeBuffer.resize(1024, 1024);

    // Depth texture
    m_shadowCubeDepthTexture = std::make_shared<CTexture>();
    m_shadowCubeDepthTexture->init(1024, 1024, GL_DEPTH_COMPONENT24);

    glBindTexture(GL_TEXTURE_2D, m_shadowCubeDepthTexture->getId());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_R32F, 1024, 1024, 0, GL_RED,
                     GL_FLOAT, NULL);
    }

    m_shadowCubeTexture = std::make_shared<CTexture>(textureId, false, 1024, 1024,
                                                     GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT);

    // Error check
    std::string error;
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowCubeBuffer.getId());
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           m_shadowCubeDepthTexture->getId(), 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    m_shadowCubeBuffer.setInactive(GL_FRAMEBUFFER);

    // Error check
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
        return false;
    }

    LOG_INFO("Shadow cube buffer state: %s.", m_shadowCubeBuffer.getState().c_str());

    // Reset framebuffer
    m_shadowCubeBuffer.setInactive(GL_FRAMEBUFFER);

    // Error check
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
        return false;
    }
    return true;
}

bool CDeferredRenderer::initShadowMapPass(IResourceManager* manager)
{
    std::string shaderFile("data/shader/shadow_map_pass.ini");
    m_shadowMapPassShaderId = manager->loadShader(shaderFile);

    if (m_shadowMapPassShaderId == invalidResource)
    {
        LOG_ERROR("Failed to initialize the shader from file %s.", shaderFile.c_str());
        return false;
    }

    // Depth texture
    m_shadowDepthTexture = std::make_shared<CTexture>();
    m_shadowDepthTexture->init(4000, 4000, GL_DEPTH_COMPONENT24);

    glBindTexture(GL_TEXTURE_2D, m_shadowDepthTexture->getId());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Error check
    std::string error;
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
        return false;
    }

    // Total 96 bit per pixel
    m_shadowMapBuffer.attach(m_shadowDepthTexture, GL_DEPTH_ATTACHMENT);
    m_shadowMapBuffer.setActive(GL_FRAMEBUFFER);
    glDrawBuffer(GL_NONE);
    m_shadowMapBuffer.setInactive(GL_FRAMEBUFFER);

    // Error check
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
        return false;
    }

    LOG_INFO("Shadow map buffer state: %s.", m_shadowMapBuffer.getState().c_str());

    // Reset framebuffer
    m_shadowMapBuffer.setInactive(GL_FRAMEBUFFER);

    // Error check
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
        return false;
    }
    return true;
}

bool CDeferredRenderer::initPointLightPass(IResourceManager* manager)
{
    // Point light shader
    std::string pointLightPassShaderFile("data/shader/deferred/point_light_pass.ini");
    m_pointLightPassShaderId = manager->loadShader(pointLightPassShaderFile);

    // Check if ok
    if (m_pointLightPassShaderId == invalidResource)
    {
        LOG_ERROR("Failed to initialize the shader from file %s.",
                  pointLightPassShaderFile.c_str());
        return false;
    }

    // Load sphere mesh for point light representation
    std::string sphereMesh = "data/mesh/sphere.obj";
    m_pointLightSphereId = manager->loadMesh(sphereMesh);
    if (m_pointLightSphereId == invalidResource)
    {
        LOG_ERROR("Failed to load point light volume mesh %s.", sphereMesh.c_str());
        return false;
    }

    // Attach texture for light data
    m_lightPassTexture = std::make_shared<CTexture>();
    if (!m_lightPassTexture->init(800, 600, GL_RGBA16F))
    {
        LOG_ERROR("Failed to create color texture for light pass frame buffer.");
        return false;
    }
    m_lightPassFrameBuffer.attach(m_lightPassTexture, GL_COLOR_ATTACHMENT0);

    // Attach depth buffer
    // Depth gets discarded, depth values from geometry pass are used
    std::shared_ptr<CRenderBuffer> depthBuffer = std::make_shared<CRenderBuffer>();
    if (!depthBuffer->init(800, 600, GL_DEPTH_COMPONENT))
    {
        LOG_ERROR("Failed to create depth buffer for light pass frame buffer.");
        return false;
    }
    m_lightPassFrameBuffer.attach(depthBuffer, GL_DEPTH_ATTACHMENT);
    return true;
}

bool CDeferredRenderer::initDirectionalLightPass(IResourceManager* manager)
{
    // Uses same frame buffer as point light pass
    // Directional light shader
    std::string directionalLightPassShaderFile("data/shader/deferred/directional_light_pass.ini");
    m_directionalLightPassShaderId = manager->loadShader(directionalLightPassShaderFile);

    // Check if ok
    if (m_directionalLightPassShaderId == invalidResource)
    {
        LOG_ERROR("Failed to initialize the shader from file %s.",
                  directionalLightPassShaderFile.c_str());
        return false;
    }

    // Load quad mesh for directional light representation
    std::string quadMesh = "data/mesh/screen_quad.obj";
    m_directionalLightScreenQuadId = manager->loadMesh(quadMesh);
    if (m_directionalLightScreenQuadId == invalidResource)
    {
        LOG_ERROR("Failed to load screen quad mesh %s.", quadMesh.c_str());
        return false;
    }

    return true;
}

bool CDeferredRenderer::initIlluminationPass(IResourceManager* manager)
{
    // Error check
    std::string error;
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
        return false;
    }
    // Load illumination shader
    std::string illuminationPassShaderFile = "data/shader/deferred/illumination_pass.ini";
    m_illuminationPassShaderId = manager->loadShader(illuminationPassShaderFile);
    // Check if ok
    if (m_illuminationPassShaderId == invalidResource)
    {
        LOG_ERROR("Failed to initialize the shader from file %s.",
                  illuminationPassShaderFile.c_str());
        return false;
    }

    // Error check
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
        return false;
    }

    // Screen quad mesh
    std::string quadMesh = "data/mesh/screen_quad.obj";
    m_illuminationPassScreenQuadId = manager->loadMesh(quadMesh);
    // Check if ok
    if (m_illuminationPassScreenQuadId == invalidResource)
    {
        LOG_ERROR("Failed to load screen quad mesh %s.", quadMesh.c_str());
        return false;
    }

    // Error check
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
        return false;
    }

    // FBO
    m_illuminationPassTexture = std::make_shared<CTexture>();
    if (!m_illuminationPassTexture->init(800, 600, GL_RGB))
    {
        LOG_ERROR("Failed to initialize illumination pass texture.");
        return false;
    }
    m_illumationPassFrameBuffer.attach(m_illuminationPassTexture, GL_COLOR_ATTACHMENT0);

    // TODO Check FBO

    // Error check
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
        return false;
    }

    return true;
}

bool CDeferredRenderer::initPostProcessPass(IResourceManager* manager)
{
    // Gauss blur pass
    if (!initGaussBlurHorizontalPass(manager))
    {
        LOG_ERROR("Failed to initialize gauss blur horizontal pass.");
        return false;
    }
    if (!initGaussBlurVerticalPass(manager))
    {
        LOG_ERROR("Failed to initialize gauss blur vertical pass.");
        return false;
    }

    // FXAA
    if (!initFxaaPass(manager))
    {
        LOG_ERROR("Failed to initialize fxaa pass.");
        return false;
    }

    // Fog
    if (!initFogPass(manager))
    {
        LOG_ERROR("Failed to initialize fog pass.");
        return false;
    }

	// Godray
    if (!initGodRayPass1(manager))
    {
        LOG_ERROR("Failed to initialize god ray 1 pass.");
        return false;
    }

	// Should actually compose by additive blending
    if (!initGodRayPass2(manager))
    {
        LOG_ERROR("Failed to initialize god ray 2 pass.");
        return false;
    }

    // Screen quad mesh
    std::string quadMesh = "data/mesh/screen_quad.obj";
    m_postProcessScreenQuadId = manager->loadMesh(quadMesh);
    if (m_directionalLightScreenQuadId == invalidResource)
    {
        LOG_ERROR("Failed to load screen quad mesh %s.", quadMesh.c_str());
        return false;
    }

    // Init post processing FBOs
    // Texture 0
    m_postProcessPassTexture0 = std::make_shared<CTexture>();
    if (!m_postProcessPassTexture0->init(800, 600, GL_RGB))
    {
        LOG_ERROR("Failed to initialize post process pass texture 0.");
        return false;
    }

    // Texture 1
    m_postProcessPassTexture1 = std::make_shared<CTexture>();
    if (!m_postProcessPassTexture1->init(800, 600, GL_RGB))
    {
        LOG_ERROR("Failed to initialize post process pass texture 1.");
        return false;
    }

    // Texture 2
    m_postProcessPassTexture2 = std::make_shared<CTexture>();
    if (!m_postProcessPassTexture2->init(800, 600, GL_RGB))
    {
        LOG_ERROR("Failed to initialize post process pass texture 2.");
        return false;
    }

    m_postProcessPassFrameBuffer0.attach(m_postProcessPassTexture0, GL_COLOR_ATTACHMENT0);
    m_postProcessPassFrameBuffer1.attach(m_postProcessPassTexture1, GL_COLOR_ATTACHMENT0);
    m_postProcessPassFrameBuffer2.attach(m_postProcessPassTexture2, GL_COLOR_ATTACHMENT0);
    // TODO Depth attachment required?

    if (!initDepthOfFieldPass(manager))
    {
        LOG_ERROR("Failed to initialize depth of field pass.");
        return false;
    }
    return true;
}

bool CDeferredRenderer::initDepthOfFieldPass(IResourceManager* manager)
{
    // Depth of field shader
    std::string depthOfFieldShaderFile = "data/shader/post/depth_of_field_pass.ini";
    m_depthOfFieldPassShaderId = manager->loadShader(depthOfFieldShaderFile);
    // Check if ok
    if (m_depthOfFieldPassShaderId == invalidResource)
    {
        LOG_ERROR("Failed to initialize the shader from file %s.", depthOfFieldShaderFile.c_str());
        return false;
    }
    return true;
}

bool CDeferredRenderer::initGaussBlurVerticalPass(IResourceManager* manager)
{
    // Depth of field shader
    std::string gaussBlurVerticalShaderFile = "data/shader/post/gauss_blur_vertical_pass.ini";
    m_gaussBlurVerticalShaderId = manager->loadShader(gaussBlurVerticalShaderFile);
    // Check if ok
    if (m_gaussBlurVerticalShaderId == invalidResource)
    {
        LOG_ERROR("Failed to initialize the shader from file %s.",
                  gaussBlurVerticalShaderFile.c_str());
        return false;
    }
    return true;
}

bool CDeferredRenderer::initGaussBlurHorizontalPass(IResourceManager* manager)
{
    // Depth of field shader
    std::string gaussBlurHorizontalShaderFile = "data/shader/post/gauss_blur_horizontal_pass.ini";
    m_gaussBlurHorizontalShaderId = manager->loadShader(gaussBlurHorizontalShaderFile);
    // Check if ok
    if (m_gaussBlurHorizontalShaderId == invalidResource)
    {
        LOG_ERROR("Failed to initialize the shader from file %s.",
                  gaussBlurHorizontalShaderFile.c_str());
        return false;
    }
    return true;
}

bool CDeferredRenderer::initFxaaPass(IResourceManager* manager)
{
    // Fxaa shader
    std::string fxaaShaderFile = "data/shader/post/fxaa_pass.ini";
    m_fxaaPassShaderId = manager->loadShader(fxaaShaderFile);
    // Check if ok
    if (m_fxaaPassShaderId == invalidResource)
    {
        LOG_ERROR("Failed to initialize the shader from file %s.", fxaaShaderFile.c_str());
        return false;
    }
    return true;
}

bool CDeferredRenderer::initFogPass(IResourceManager* manager)
{
    // Fog shader
    std::string fogShaderFile = "data/shader/post/fog_pass.ini";
    m_fogPassShaderId = manager->loadShader(fogShaderFile);
    // Check if ok
    if (m_fogPassShaderId == invalidResource)
    {
        LOG_ERROR("Failed to initialize the shader from file %s.", fogShaderFile.c_str());
        return false;
    }
    return true;
}

bool CDeferredRenderer::initGodRayPass1(IResourceManager* manager)
{
    // Get shader
    std::string shader = "data/shader/post/god_ray_1_pass.ini";
    m_godRayPass1ShaderId = manager->loadShader(shader);
    // Check if ok
    if (m_godRayPass1ShaderId == invalidResource)
    {
        LOG_ERROR("Failed to initialize the shader from file %s.", shader.c_str());
        return false;
    }
    return true;
}

bool CDeferredRenderer::initGodRayPass2(IResourceManager* manager)
{
    // Get shader
    std::string shader = "data/shader/post/god_ray_2_pass.ini";
    m_godRayPass2ShaderId = manager->loadShader(shader);
    // Check if ok
    if (m_godRayPass2ShaderId == invalidResource)
    {
        LOG_ERROR("Failed to initialize the shader from file %s.", shader.c_str());
        return false;
    }
    return true;
}

bool CDeferredRenderer::initDisplayPass(IResourceManager* manager)
{
    // Display shader
    std::string displayPassShaderFile = "data/shader/display_pass.ini";
    m_displayPassShaderId = manager->loadShader(displayPassShaderFile);
    // Check if ok
    if (m_displayPassShaderId == invalidResource)
    {
        LOG_ERROR("Failed to initialize the shader from file %s.", displayPassShaderFile.c_str());
        return false;
    }
    return true;
}

bool CDeferredRenderer::initVisualizeDepthPass(IResourceManager* manager)
{
    // Get shader
    std::string shaderFile = "data/shader/debug/visualize_depth_buffer_pass.ini";
    m_visualizeDepthPassShaderId = manager->loadShader(shaderFile);
    // Check if ok
    if (m_visualizeDepthPassShaderId == invalidResource)
    {
        LOG_ERROR("Failed to initialize the shader from file %s.", shaderFile.c_str());
        return false;
    }
    return true;
}
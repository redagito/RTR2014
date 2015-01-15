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

    if (!m_screenQuadPass.init(manager))
    {
        LOG_ERROR("Failed to initialize screen quad pass.");
        return false;
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

    // Final pass, creates lit scene from lbuffer and gbuffer
    m_screenQuadPass.draw(m_diffuseGlowTexture.get(), m_lightPassTexture.get(),
                          m_depthTexture.get(), m_transformer.getInverseViewProjectionMatrix(),
                          nullptr, &manager);

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

    // Set screen size
    pointLightPassShader->setUniform(screenWidthUniformName, (float)window.getWidth());
    pointLightPassShader->setUniform(screenHeightUniformName, (float)window.getHeight());

    // Inverse view-projection
    pointLightPassShader->setUniform(inverseViewProjectionMatrixUniformName,
                                     m_transformer.getInverseViewProjectionMatrix());

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

        if (!scene.getPointLight(pointLightId, position, radius, color, intensity))
        {
            LOG_ERROR("Failed to retrieve point light data from point light id %i.", pointLightId);
        }
        else
        {
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
    // Directional light pass
    // Reset culling
    glCullFace(GL_BACK);

    // Restrieve shader
    CShaderProgram* directionalLightPassShader =
        manager.getShaderProgram(m_directionalLightPassShaderId);
    return;
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
    std::string geometryPassShaderFile("data/shader/deferred_geometry_pass.ini");
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

bool CDeferredRenderer::initPointLightPass(IResourceManager* manager)
{
    // Point light shader
    std::string pointLightPassShaderFile("data/shader/deferred_point_light_pass.ini");
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
    std::string directionalLightPassShaderFile("data/shader/deferred_directional_light_pass.ini");
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
        LOG_ERROR("Failed to load point light volume mesh %s.", quadMesh.c_str());
        return false;
    }

	return true;
}
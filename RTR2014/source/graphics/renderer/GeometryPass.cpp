#include "graphics/renderer/GeometryPass.h"

#include <spdlog/spdlog.h>

#include <string>

#include "graphics/ICamera.h"
#include "graphics/IGraphicsResourceManager.h"
#include "graphics/IScene.h"
#include "graphics/ISceneQuery.h"
#include "graphics/IWindow.h"
#include "graphics/Material.h"
#include "graphics/Mesh.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Texture.h"
#include "graphics/renderer/Renderer.h"
#include "graphics/renderer/core/RendererCoreConfig.h"
#include "resource/IResourceManager.h"

GeometryPass::GeometryPass() { return; }

GeometryPass::~GeometryPass() { return; }

void GeometryPass::render(const IScene& scene, ISceneQuery& query, const ICamera& camera,
                          const IWindow& window, const IGraphicsResourceManager& manager)
{
    // Reset viewport
    glViewport(0, 0, window.getWidth(), window.getHeight());
    m_geometryBuffer.resize(window.getWidth(), window.getHeight());
    // Set framebuffer
    m_geometryBuffer.setActive(GL_FRAMEBUFFER);
    // Clear buffer
    // TODO Should be retrieved as scene parameters
    // Diffuse, glow
    GLfloat diffuseGlow[] = {0.f, 0.f, 0.f, 0.f};
    // Normals are stored as raw float [-1,1], so the clear value is a plain
    // straight-up normal, not an 8-bit-encoded one.
    GLfloat normalSpecular[] = {0.f, 0.f, 1.f, 0.f};
    glClearBufferfv(GL_COLOR, 0, diffuseGlow);
    // Normal, specular
    glClearBufferfv(GL_COLOR, 1, normalSpecular);

    // Clear
    glClear(GL_DEPTH_BUFFER_BIT);

    // The sky fills the background, it is drawn before the scene.
    m_skyPass.render(window, manager, camera);

    // Geometry pass, uses gbuffer fbo
    ShaderProgram* geometryPassShader = manager.getShaderProgram(m_geometryPassShaderId);
    if (geometryPassShader == nullptr)
    {
        spdlog::error("Shader program for geometry pass could not be retrieved.");
        return;
    }

    // Depth
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Backface culling disabled for debugging
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Winding order, standard is counter-clockwise
    glFrontFace(GL_CCW);

    // Send view/projection to default shader
    geometryPassShader->setUniform(viewMatrixUniformName, camera.getView());
    geometryPassShader->setUniform(projectionMatrixUniformName, camera.getProjection());
    geometryPassShader->setUniform(normalMappingActiveUniformName,
                                   camera.getFeatureInfo().normalMappingActive ? 1 : 0);

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
        bool castsShadow = true;

        // Retrieve object data
        if (!scene.getObject(id, meshId, materialId, position, rotation, scale, castsShadow))
        {
            // Invalid id
            spdlog::error("Invalid scene object id {}.", id);
        }
        else
        {
            // Resolve ids
            Mesh* mesh = manager.getMesh(meshId);
            Material* material = manager.getMaterial(materialId);
            if (mesh == nullptr || material == nullptr)
            {
                // The graphics resource manager logged the missing resource
                continue;
            }

            // Set transformations
            m_transformer.setPosition(position);
            m_transformer.setRotation(rotation);
            m_transformer.setScale(scale);

            if (material->hasCustomShader())
            {
                // Custom shaders not supported
                spdlog::warn("Deferred renderer does not support custom material shaders.");
            }
            else
            {
                // Forward draw call
                geometryPassShader->setActive();
                Renderer::bindMaterial(
                    geometryPassShader, material, m_transformer.getTranslationMatrix(),
                    m_transformer.getRotationMatrix(), m_transformer.getScaleMatrix(), manager);
                Renderer::draw(mesh);
            }
        }
    }

    // Post draw error check

    // Disable geometry buffer
    m_geometryBuffer.setInactive(GL_FRAMEBUFFER);
}
bool GeometryPass::init(IResourceManager* manager, const std::string& skyCubemapFile)
{
    // Init geometry pass shader
    // TODO Read file name from config?
    // Geometry pass shader for filling gbuffer
    std::string geometryPassShaderFile("data/shader/deferred/geometry_pass.ini");
    m_geometryPassShaderId = manager->loadShader(geometryPassShaderFile);

    // Check if ok
    if (m_geometryPassShaderId == invalidResource)
    {
        spdlog::error("Failed to initialize the shader from file {}.",
                      geometryPassShaderFile.c_str());
        return false;
    }

    // The sky that fills the background of the scene
    if (!m_skyPass.init(manager, skyCubemapFile))
    {
        spdlog::error("Failed to initialize the sky.");
        return false;
    }

    // Init gbuffer
    // Diffuse texture, stores base color and glow mask.
    m_diffuseGlowTexture = std::make_shared<Texture>();
    if (!m_diffuseGlowTexture->init(800, 600, GL_RGBA8))
    {
        spdlog::error("Failed to initialize the diffuse and glow texture.");
        return false;
    }

    // Normal texture, store x and y, z normals and specularity.
    m_normalSpecularTexture = std::make_shared<Texture>();
    if (!m_normalSpecularTexture->init(800, 600, GL_RGBA16F))
    {
        spdlog::error("Failed to initialize the normal and specular texture.");
        return false;
    }

    // Depth texture
    m_depthTexture = std::make_shared<Texture>();
    if (!m_depthTexture->init(800, 600, GL_DEPTH_COMPONENT24))
    {
        spdlog::error("Failed to initialize the depth texture.");
        return false;
    }

    // Total 96 bit per pixel
    m_geometryBuffer.attach(m_depthTexture, GL_DEPTH_ATTACHMENT);
    m_geometryBuffer.attach(m_diffuseGlowTexture, GL_COLOR_ATTACHMENT0);
    m_geometryBuffer.attach(m_normalSpecularTexture, GL_COLOR_ATTACHMENT1);

    spdlog::info("GBuffer state: {}.", m_geometryBuffer.getState().c_str());

    // Reset framebuffer
    m_geometryBuffer.setInactive(GL_FRAMEBUFFER);

    return true;
}

const std::shared_ptr<Texture>& GeometryPass::getDepthTexture() const { return m_depthTexture; }

const std::shared_ptr<Texture>& GeometryPass::getDiffuseGlowTexture() const
{
    return m_diffuseGlowTexture;
}

const std::shared_ptr<Texture>& GeometryPass::getNormalSpecularTexture() const
{
    return m_normalSpecularTexture;
}
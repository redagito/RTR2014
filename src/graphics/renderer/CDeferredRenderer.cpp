#include "CDeferredRenderer.h"

#include <cassert>
#include <string>

#include <glm/ext.hpp>

#include "graphics/IScene.h"
#include "graphics/ISceneQuery.h"
#include "graphics/ICamera.h"
#include "graphics/IWindow.h"
#include "graphics/IResourceManager.h"

#include "core/RendererCoreConfig.h"

#include "debug/RendererDebug.h"
#include "debug/Log.h"

CDeferredRenderer::CDeferredRenderer(const std::shared_ptr<IResourceManager>& resourceManager)
    : ARenderer(resourceManager)
{
    // Set clear color
    glClearColor(0.6f, 0.6f, 0.6f, 1.0f);

    // Depth
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Backface culling disabled for debugging
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Winding order, standard is counter-clockwise
    glFrontFace(GL_CCW);

    // Error check
    std::string error;
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
    }
    return;
}

CDeferredRenderer::~CDeferredRenderer() { return; }

bool CDeferredRenderer::init()
{
    m_colorTexture = std::make_shared<CTexture>();
    m_depthTexture = std::make_shared<CTexture>();

    m_colorTexture->init(800, 600, GL_RGB8);
    m_depthTexture->init(800, 600, GL_DEPTH_COMPONENT16);

    m_frameBuffer.attach(m_colorTexture, GL_COLOR_ATTACHMENT0);
    m_frameBuffer.attach(m_depthTexture, GL_DEPTH_ATTACHMENT);

    LOG_INFO("Framebuffer state: %s.", m_frameBuffer.getState().c_str());

    GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffers);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Error check
    std::string error;
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
    }

    return true;
}

void CDeferredRenderer::draw(const IScene& scene, const ICamera& camera, const IWindow& window)
{
    // Draw init
    window.setActive();

    // Set framebuffer
    m_frameBuffer.setActive(GL_FRAMEBUFFER);

    // Clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset viewport
    glViewport(0, 0, window.getWidth(), window.getHeight());

    // Set view and projection matrices
    m_currentView = camera.getView();
    m_currentProjection = camera.getProjection();

    // Query visible scene objects
    std::unique_ptr<ISceneQuery> query(std::move(scene.createQuery(camera)));

    // Send view/projection to default shader
    m_defaultShader->setUniform(viewMatrixUniformName, m_currentView);
    m_defaultShader->setUniform(projectionMatrixUniformName, m_currentProjection);

    // Traverse visible objects
    while (query->hasNextObject())
    {
        // Get next visible object
        SceneObjectId id = query->getNextObject();

        // Object attributes
        ResourceId mesh = -1;
        ResourceId material = -1;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        // Retrieve object data
        if (!scene.getObject(id, mesh, material, position, rotation, scale))
        {
            // Invalid id
            LOG_ERROR("Invalid scene object id %l.", id);
        }
        else
        {
            // Forward draw call, stores render requests with custom shader set in material
            draw(mesh, position, rotation, scale, material);
        }
    }

    // Post draw error check
    std::string error;
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
    }
}

void CDeferredRenderer::draw(ResourceId meshId, const glm::vec3& position,
                             const glm::vec3& rotation, const glm::vec3& scale,
                             ResourceId materialId)
{
    // Resolve ids
    CMesh* mesh = getMesh(meshId);
    CMaterial* material = getMaterial(materialId);

    // Create matrices
    glm::mat4 translationMatrix = glm::translate(position);

    // TODO Correct but slow?
    glm::mat4 rotationMatrix = glm::rotate(rotation.x, glm::vec3(1.f, 0.f, 0.f)) *
                               glm::rotate(rotation.y, glm::vec3(0.f, 1.f, 0.f)) *
                               glm::rotate(rotation.z, glm::vec3(0.f, 0.f, 1.f));
    glm::mat4 scaleMatrix = glm::scale(scale);

    // Forward draw call
    draw(mesh, translationMatrix, rotationMatrix, scaleMatrix, material);
}

void CDeferredRenderer::draw(CMesh* mesh, const glm::mat4& translation, const glm::mat4& rotation,
                             const glm::mat4& scale, CMaterial* material)
{
    // Decide which shader program to use
    CShaderProgram* shader = m_defaultShader;
    if (material->hasCustomShader())
    {
        shader = material->getCustomShader();
    }
    shader->setActive();

    // Custom shader needs access to view/projection matrices
    if (material->hasCustomShader())
    {
        shader->setUniform(viewMatrixUniformName, m_currentView);
        shader->setUniform(projectionMatrixUniformName, m_currentProjection);
    }

    // Transformation matrices
    shader->setUniform(translationMatrixUniformName, translation);
    shader->setUniform(rotationMatrixUniformName, rotation);
    shader->setUniform(scaleMatrixUniformName, scale);
    shader->setUniform(modelMatrixUniformName, translation * rotation * scale);

    // Send material textures to shader
    if (material->hasDiffuse())
    {
        material->getDiffuse()->setActive(diffuseTextureUnit);
    }
    else
    {
        m_defaultDiffuseTexture->setActive(diffuseTextureUnit);
    }
    shader->setUniform(diffuseTextureUniformName, diffuseTextureUnit);

    if (material->hasNormal())
    {
        material->getNormal()->setActive(normalTextureUnit);
    }
    else
    {
        // Use default texture
        m_defaultNormalTexture->setActive(normalTextureUnit);
    }
    shader->setUniform(normalTextureUniformName, normalTextureUnit);

    if (material->hasSpecular())
    {
        material->getSpecular()->setActive(specularTextureUnit);
    }
    else
    {
        // Use default texture
        m_defaultSpecularTexture->setActive(specularTextureUnit);
    }
    shader->setUniform(specularTextureUniformName, specularTextureUnit);

    if (material->hasGlow())
    {
        material->getGlow()->setActive(glowTextureUnit);
    }
    else
    {
        // Use default texture
        m_defaultGlowTexture->setActive(glowTextureUnit);
    }
    shader->setUniform(glowTextureUniformName, glowTextureUnit);

    // TODO Sort materials with alpha texture for blending.
    if (material->hasAlpha())
    {
        material->getAlpha()->setActive(alphaTextureUnit);
    }
    else
    {
        m_defaultAlphaTexture->setActive(alphaTextureUnit);
    }
    shader->setUniform(alphaTextureUniformName, alphaTextureUnit);

    // Draw mesh
    // TODO Consider custom shader bindings for meshes
    ARenderer::draw(mesh);

    // TODO Cleanup?
}

bool CDeferredRenderer::initDefaultShaders()
{
    // TODO Read file name from config?
    std::string defaultShaderFile("data/shader/shader_test_0.ini");

    ResourceId shaderId = getResourceManager()->loadShader(defaultShaderFile);
    m_defaultShader = getShaderProgram(shaderId);

    // Check if ok
    if (m_defaultShader == nullptr)
    {
        LOG_ERROR("Failed to initialize the default shader from file %s.",
                  defaultShaderFile.c_str());
        return false;
    }
    return true;
}

#include "CDeferredRenderer.h"

#include <cassert>
#include <string>

#include <glm/ext.hpp>

#include "graphics/IScene.h"
#include "graphics/ISceneQuery.h"
#include "graphics/ICamera.h"
#include "graphics/IWindow.h"
#include "resource/IResourceManager.h"

#include "core/RendererCoreConfig.h"

#include "debug/RendererDebug.h"
#include "debug/Log.h"

CDeferredRenderer::CDeferredRenderer(const std::shared_ptr<IResourceManager>& resourceManager)
	: ARenderer(resourceManager), m_fullscreenQuadShader(nullptr)
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
	LOG_INFO("Initializing deferred renderer.");

	// Init gbuffer

	// Diffuse texture, stores base color.
	// Uses 24 bit per pixel
	m_diffuseTexture = std::make_shared<CTexture>();
	m_diffuseTexture->init(800, 600, GL_RGB8);

	// Normal texture, 2 half-floats store x and y, z can be calculated as the vector is normalized.
	// Uses 32 bit per pixel
	m_normalTexture = std::make_shared<CTexture>();
	m_normalTexture->init(800, 600, GL_RG16F);
	
	// Depth texture with 24 bit precision
	// Uses 24 bit per pixel
	m_depthTexture->init(800, 600, GL_DEPTH_COMPONENT24);
	m_depthTexture = std::make_shared<CTexture>();

	// Texture with glow and specular data. Stores 8 bit glow and 8 bit specularity
	// Uses 16 bit per pixel
	m_glowSpecularTexture->init(800, 600, GL_RG8);
	m_glowSpecularTexture = std::make_shared<CTexture>();

	// Total 96 bit per pixel
	m_frameBuffer.attach(m_depthTexture, GL_DEPTH_ATTACHMENT);
	m_frameBuffer.attach(m_diffuseTexture, GL_COLOR_ATTACHMENT0);
	m_frameBuffer.attach(m_normalTexture, GL_COLOR_ATTACHMENT1);
	m_frameBuffer.attach(m_glowSpecularTexture, GL_COLOR_ATTACHMENT2);

    LOG_INFO("Framebuffer state: %s.", m_frameBuffer.getState().c_str());

	// Set color attachments for geometry pass fbo
	m_drawBuffers[0] = GL_COLOR_ATTACHMENT0;
	m_drawBuffers[1] = GL_COLOR_ATTACHMENT1;
	m_drawBuffers[2] = GL_COLOR_ATTACHMENT2;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Screen space quad pass

	// Create dummy vbo
	m_dummy = std::make_shared<CVertexBuffer>(std::vector<float>({0.f, 0.f, 0.f}));

    // Error check
    std::string error;
    if (hasGLError(error))
    {
        LOG_ERROR("GL Error: %s", error.c_str());
		return false;
    }

	if (!initShaders())
	{
		LOG_ERROR("Failed to initialize shaders for deferred renderer.");
		return false;
	}

    return true;
}

void CDeferredRenderer::draw(const IScene& scene, const ICamera& camera, const IWindow& window)
{
    // Draw init
    window.setActive();

	// Geometry pass, uses gbuffer fbo

    // Set framebuffer
    m_frameBuffer.setActive(GL_FRAMEBUFFER);
	// Set render targets
	glDrawBuffers(3, m_drawBuffers);

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
    m_geometryPassShader->setUniform(viewMatrixUniformName, m_currentView);
	m_geometryPassShader->setUniform(projectionMatrixUniformName, m_currentProjection);

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

	// Geometry pass end, gbuffer populated

	// Postprocessing done here

	// Draw fullscreen quad
	m_fullscreenQuadShader->setActive();
	// Bind texture
	m_diffuseTexture->setActive(0);
	m_fullscreenQuadShader->setUniform("colorTexture", 0);
	m_dummy->setActive();
	glDrawArrays(GL_POINTS, 0, 1);

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

	if (material->hasCustomShader())
	{
		// Custom shaders not supported
		LOG_WARNING("Deferred renderer does not support custom material shaders.");
		return;
	}

    // Forward draw call
    draw(mesh, translationMatrix, rotationMatrix, scaleMatrix, material);
}

void CDeferredRenderer::draw(CMesh* mesh, const glm::mat4& translation, const glm::mat4& rotation,
                             const glm::mat4& scale, CMaterial* material)
{
    // Decide which shader program to use
    CShaderProgram* shader = m_geometryPassShader;
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

bool CDeferredRenderer::initShaders()
{
    // TODO Read file name from config?

	// Screen space quad shader
    std::string screenQuadShaderFile("data/shader/compose_screenquad.ini");
	ResourceId shaderId = getResourceManager()->loadShader(screenQuadShaderFile);
    m_fullscreenQuadShader = getShaderProgram(shaderId);
    // Check if ok
	if (m_fullscreenQuadShader == nullptr)
    {
        LOG_ERROR("Failed to initialize the shader from file %s.",
			screenQuadShaderFile.c_str());
        return false;
    }

	// Geometry pass shader for filling gbuffer
	std::string geometryPassShaderFile("data/shader/deferred_geometrypass.ini");
	shaderId = getResourceManager()->loadShader(geometryPassShaderFile);
	m_geometryPassShader = getShaderProgram(shaderId);
	// Check if ok
	if (m_fullscreenQuadShader == nullptr)
	{
		LOG_ERROR("Failed to initialize the shader from file %s.",
			geometryPassShaderFile.c_str());
		return false;
	}

    return true;
}

void CDeferredRenderer::drawFullscreenQuad(CTexture* texture)
{
	m_fullscreenQuadShader->setActive();

	texture->setActive(0);
	m_fullscreenQuadShader->setUniform("color_texture", 0);

	m_dummy->setActive();
	glDrawArrays(GL_POINTS, 0, 1);

}
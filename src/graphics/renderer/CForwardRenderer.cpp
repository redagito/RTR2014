#include "CForwardRenderer.h"

#include <cassert>
#include <string>

#include <glm/ext.hpp>

#include "graphics/IScene.h"
#include "graphics/ISceneQuery.h"
#include "graphics/ICamera.h"
#include "graphics/IWindow.h"
#include "graphics/IGraphicsResourceManager.h"

#include "graphics/resource/CMaterial.h"
#include "graphics/resource/CMesh.h"
#include "graphics/resource/CShaderProgram.h"
#include "graphics/resource/CTexture.h"

#include "resource/IResourceManager.h"

#include "core/RendererCoreConfig.h"

#include "debug/RendererDebug.h"
#include "debug/Log.h"

CForwardRenderer::CForwardRenderer()
{
    return;
}

CForwardRenderer::~CForwardRenderer()
{
    // TODO Cleanup
}

bool CForwardRenderer::init(IResourceManager* manager)
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
		return false;
	}

    // Load and init default shaders
    return initDefaultShaders(manager);
}

void CForwardRenderer::draw(const IScene& scene, const ICamera& camera, const IWindow& window, const IGraphicsResourceManager& manager)
{
	// Draw init
	window.setActive();

	// Set forward shader as active
	m_currentShader = manager.getShaderProgram(m_forwardShader);

	// Initializiation
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Reset viewport
	glViewport(0, 0, window.getWidth(), window.getHeight());

	// Set view and projection matrices
	m_currentView = camera.getView();
	m_currentProjection = camera.getProjection();

	// Query visible scene objects
	std::unique_ptr<ISceneQuery> query(std::move(scene.createQuery(camera)));

	// Send view/projection to default shader
	m_currentShader->setUniform(viewMatrixUniformName, m_currentView);
	m_currentShader->setUniform(projectionMatrixUniformName, m_currentProjection);

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

			// Create matrices
			glm::mat4 translationMatrix = glm::translate(position);

			// TODO Correct but slow?
			glm::mat4 rotationMatrix = glm::rotate(rotation.x, glm::vec3(1.f, 0.f, 0.f)) *
				glm::rotate(rotation.y, glm::vec3(0.f, 1.f, 0.f)) *
				glm::rotate(rotation.z, glm::vec3(0.f, 0.f, 1.f));
			glm::mat4 scaleMatrix = glm::scale(scale);

			// Forward draw call
			draw(mesh, translationMatrix, rotationMatrix, scaleMatrix, material, manager);
		}
	}

	// Post draw error check
	std::string error;
	if (hasGLError(error))
	{
		LOG_ERROR("GL Error: %s", error.c_str());
	}
}

CForwardRenderer* CForwardRenderer::create(IResourceManager* manager)
{
	CForwardRenderer* renderer = new CForwardRenderer;
	if (!renderer->init(manager))
	{
		delete renderer;
		renderer = nullptr;
		LOG_ERROR("Failed to create forward renderer, initialization failed.");
	}
	return renderer;
}

void CForwardRenderer::draw(CMesh* mesh, const glm::mat4& translation, const glm::mat4& rotation,
                            const glm::mat4& scale, CMaterial* material, const IGraphicsResourceManager& manager)
{
    // Decide which shader program to use
	CShaderProgram* shader = m_currentShader;
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
		manager.getDefaultAlphaTexture()->setActive(alphaTextureUnit);
    }
    shader->setUniform(alphaTextureUniformName, alphaTextureUnit);

    // Draw mesh
    // TODO Consider custom shader bindings for meshes
    ARenderer::draw(mesh);

    // TODO Cleanup?
}

bool CForwardRenderer::initDefaultShaders(IResourceManager* manager)
{
    // TODO Read file name from config?
    std::string defaultShaderFile("data/shader/forward_test_0.ini");
	m_forwardShader = manager->loadShader(defaultShaderFile);

    // Check if ok
	if (m_forwardShader == invalidResource)
    {
        LOG_ERROR("Failed to initialize the default shader from file %s.",
                  defaultShaderFile.c_str());
        return false;
    }
    return true;
}

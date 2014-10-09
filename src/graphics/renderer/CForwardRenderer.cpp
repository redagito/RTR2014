#include "CForwardRenderer.h"

#include <cassert>
#include <string>

#include <glm/ext.hpp>

#include "debug/Log.h"

#include "graphics/IScene.h"
#include "graphics/ISceneQuery.h"
#include "graphics/ICamera.h"
#include "graphics/IWindow.h"
#include "graphics/IResourceManager.h"

#include "MaterialConfig.h"
#include "debug/RendererDebug.h"

CForwardRenderer::CForwardRenderer(const std::shared_ptr<IResourceManager>& resourceManager)
	: 
	ARenderer(resourceManager)
{
	// Init default shaders
	initDefaultShaders();

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

CForwardRenderer::~CForwardRenderer()
{
	// TODO Cleanup
}

void CForwardRenderer::draw(const IScene& scene, const ICamera& camera, const IWindow& window)
{
	// Draw init
	window.setActive();

	// Initializiation
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset viewport
	glViewport(0, 0, window.getWidth(), window.getHeight());

	// TODO Default shader for rendering, remove later
	m_defaultShader->setUniform("view", camera.getView());
	m_defaultShader->setUniform("projection", camera.getProjection());

	// TODO Implement multi pass system

	// Query visible scene objects
	std::unique_ptr<ISceneQuery> query(std::move(scene.createQuery(camera)));

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

	// TODO Draw objects with custom shader

	// Post draw error check
	std::string error;
	if (hasGLError(error))
	{
		LOG_ERROR("GL Error: %s", error.c_str());
	}
}

void CForwardRenderer::draw(ResourceId meshId, const glm::vec3& position, const glm::vec3& rotation,
	const glm::vec3& scale, ResourceId materialId)
{
	// Resolve ids
	CMesh* mesh = getMesh(meshId);
	CMaterial* material = getMaterial(materialId);

	// Create matrices
	glm::mat4 translationMatrix = glm::translate(position);
	// TODO Rotation calculation is probably wrong
	glm::mat4 rotationMatrix = glm::mat4(1.f);  // glm::rotate(1.f, rotation);
	glm::mat4 scaleMatrix = glm::scale(scale);

	// Defer rendering for materials with custom shaders
	if (material->hasCustomShader())
	{
		m_customShaderMeshes.push_back(
			SRenderRequest(mesh, material, translationMatrix, rotationMatrix, scaleMatrix));
		return;
	}
	// Forward draw call
	draw(mesh, translationMatrix, rotationMatrix, scaleMatrix, material);
}

void CForwardRenderer::draw(CMesh* mesh, const glm::mat4& translation, const glm::mat4& rotation,
	const glm::mat4& scale, CMaterial* material)
{
	// Decide which shader program to use
	CShaderProgram* shader = m_defaultShader;
	if (material->hasCustomShader())
	{
		shader = material->getCustomShader();
	}

	shader->setActive();

	// Transformation matrices
	shader->setUniform("rotation", rotation);
	shader->setUniform("translation", translation);
	shader->setUniform("scale", scale);
	shader->setUniform("model", translation * rotation * scale);

	// Send material textures to shader
	if (material->hasDiffuse())
	{
		material->getDiffuse()->setActive(diffuseTextureUnit);
	}
	else
	{
		m_defaultDiffuseTexture->setActive(diffuseTextureUnit);
	}
	shader->setUniform("diffuse_texture", diffuseTextureUnit);

	if (material->hasNormal())
	{
		material->getNormal()->setActive(normalTextureUnit);
	}
	else
	{
		// Use default texture
		m_defaultNormalTexture->setActive(normalTextureUnit);
	}
	shader->setUniform("normal_texture", normalTextureUnit);

	if (material->hasSpecular())
	{
		material->getSpecular()->setActive(specularTextureUnit);
	}
	else
	{
		// Use default texture
		m_defaultSpecularTexture->setActive(specularTextureUnit);
	}
	shader->setUniform("specular_texture", specularTextureUnit);

	if (material->hasGlow())
	{
		material->getGlow()->setActive(glowTextureUnit);
	}
	else
	{
		// Use default texture
		m_defaultGlowTexture->setActive(glowTextureUnit);
	}
	shader->setUniform("glow_texture", glowTextureUnit);

	// TODO Sort materials with alpha texture for blending.
	if (material->hasAlpha())
	{
		material->getAlpha()->setActive(alphaTextureUnit);
	}
	else
	{
		m_defaultAlphaTexture->setActive(alphaTextureUnit);
	}
	shader->setUniform("alpha_texture", alphaTextureUnit);

	// Draw mesh
	// TODO Consider custom shader bindings for meshes
	mesh->getVertexArray()->setActive();

	// Set draw mode
	GLenum mode;
	unsigned int primitiveSize = 0;
	switch (mesh->getPrimitiveType())
	{
	case EPrimitiveType::Point:
		mode = GL_POINTS;
		primitiveSize = 1;
		break;
	case EPrimitiveType::Line:
		mode = GL_LINE;
		primitiveSize = 2;
		break;
	case EPrimitiveType::Triangle:
		mode = GL_TRIANGLES;
		primitiveSize = 3;
		break;
	default:
		LOG_ERROR("Invalid primitive type");
		return;
	}

	// Decide on draw method based on the stored data
	if (mesh->hasIndexBuffer())
	{
		// Indexed draw, probably faster
		mesh->getIndexBuffer()->setActive();
		glDrawElements(mode, mesh->getIndexBuffer()->getSize(), GL_UNSIGNED_INT, nullptr);
		mesh->getIndexBuffer()->setInactive();
	}
	else
	{
		// Slowest draw method
		glDrawArrays(mode, 0, mesh->getVertexBuffer()->getSize() / primitiveSize);
	}
	mesh->getVertexArray()->setInactive();
}

bool CForwardRenderer::initDefaultShaders()
{
	// TODO Read file name from config?
	std::string defaultShaderFile("data/shader/shader_test_0.ini");

	ResourceId shaderId = getResourceManager()->loadShader(defaultShaderFile);
	m_defaultShader = getShaderProgram(shaderId);

	// Check if ok
	if (m_defaultShader == nullptr)
	{
		LOG_ERROR("Failed to initialize the default shader from file %s.", defaultShaderFile.c_str());
		return false;
	}
	return true;
}
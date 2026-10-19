#include <spdlog/spdlog.h>
#include "graphics/renderer/ForwardRenderer.h"

#include <cassert>
#include <string>

#include <glm/ext/matrix_transform.hpp>

#include "graphics/IScene.h"
#include "graphics/ISceneQuery.h"
#include "graphics/ICamera.h"
#include "graphics/IWindow.h"
#include "graphics/IGraphicsResourceManager.h"

#include "graphics/Material.h"
#include "graphics/Mesh.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Texture.h"

#include "resource/IResourceManager.h"

#include "graphics/renderer/core/RendererCoreConfig.h"


ForwardRenderer::ForwardRenderer()
{
    return;
}

ForwardRenderer::~ForwardRenderer()
{
    // TODO Cleanup
}

bool ForwardRenderer::init(IResourceManager* manager)
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


    // Load and init default shaders
    return initDefaultShaders(manager);
}

void ForwardRenderer::draw(const IScene& scene, const ICamera& camera, const IWindow& window, const IGraphicsResourceManager& manager)
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
			spdlog::error("Invalid scene object id {}.", id);
		}
		else
		{
			// Resolve ids
			Mesh* mesh = manager.getMesh(meshId);
			Material* material = manager.getMaterial(materialId);

			// Create matrices
			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);

			// TODO Correct but slow?
			glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1.f, 0.f, 0.f)) *
				glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0.f, 1.f, 0.f)) *
				glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.f, 0.f, 1.f));
			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

			// Forward draw call
			draw(mesh, translationMatrix, rotationMatrix, scaleMatrix, material, manager);
		}
	}

	// Post draw error check
}

ForwardRenderer* ForwardRenderer::create(IResourceManager* manager)
{
	ForwardRenderer* renderer = new ForwardRenderer;
	if (!renderer->init(manager))
	{
		delete renderer;
		renderer = nullptr;
		spdlog::error("Failed to create forward renderer, initialization failed.");
	}
	return renderer;
}

void ForwardRenderer::draw(Mesh* mesh, const glm::mat4& translation, const glm::mat4& rotation,
                            const glm::mat4& scale, Material* material, const IGraphicsResourceManager& manager)
{
    // Decide which shader program to use
	ShaderProgram* shader = m_currentShader;
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

    bindMaterial(shader, material, translation, rotation, scale, manager);

    // Draw mesh
    // TODO Consider custom shader bindings for meshes
    Renderer::draw(mesh);

    // TODO Cleanup?
}

bool ForwardRenderer::initDefaultShaders(IResourceManager* manager)
{
    // TODO Read file name from config?
    std::string defaultShaderFile("data/shader/forward_test_0.ini");
	m_forwardShader = manager->loadShader(defaultShaderFile);

    // Check if ok
	if (m_forwardShader == invalidResource)
    {
        spdlog::error("Failed to initialize the default shader from file {}.",
                  defaultShaderFile.c_str());
        return false;
    }
    return true;
}

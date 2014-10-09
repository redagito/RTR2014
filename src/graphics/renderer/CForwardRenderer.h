#pragma once

#include <memory>
#include <list>

// Required by inheritance
#include "ARenderer.h"

#include "SRenderRequest.h"

/**
* \brief Forward renderer.
*/
class CForwardRenderer : public ARenderer
{
public:
	/**
	* \brief Forwards resource manager to base class.
	*/
	CForwardRenderer(const std::shared_ptr<IResourceManager>& resourceManager);

	/**
	* \brief Internal resource cleanup.
	*/
	~CForwardRenderer();

	/**
	* \brief Draws whole scene.
	* Implements the draw call as specified by the renderer interface.
	* This function implements the highlevel drawing logic for the scene,
	* selects rendering targets and manages render passes.
	*/
	void draw(const IScene& scene, const ICamera& camera, const IWindow& window);

protected:
	/**
	* \brief Utility draw function.
	* Resolves the resource ids to internal objects, constructs
	* the transformation matrices and forwards call.
	*/
	void draw(ResourceId mesh, const glm::vec3& position, const glm::vec3& rotation,
		const glm::vec3& scale, ResourceId);

	/**
	* \brief Draws mesh to current rendering target.
	* Selects the rendering method (indexed, direct, etc.) based on the available mesh data
	* and performs actual draw call.
	*/
	void draw(CMesh* mesh, const glm::mat4& translation, const glm::mat4& rotation,
		const glm::mat4& scale, CMaterial* material);

private:
	bool initDefaultShaders();

	glm::mat4 m_currentView; /**< Stores the current view matrix. */
	glm::mat4 m_currentProjection; /**< Stores the current projection matrix. */

	std::list<SRenderRequest> m_customShaderMeshes; /**< Render requests with custom shaders. */
	CShaderProgram* m_defaultShader; /**< Default shader for rendering. TODO for testing, remove later. */
};
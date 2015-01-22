#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "SceneConfig.h"
#include "resource/ResourceConfig.h"

class ICamera;
class ISceneQuery;

/**
 * \brief Scene interface class.
 * The scene should only return internal ids. This provides the foundation for
 * future threading.
 */
class IScene
{
   public:
    virtual ~IScene();

    /**
     * \brief Creates object in scene and returns id.
     */
    virtual SceneObjectId createObject(ResourceId mesh, ResourceId material,
                                       const glm::vec3& position, const glm::vec3& rotation,
                                       const glm::vec3& scale) = 0;

    /**
     * \brief Returns scene object data.
     */
    virtual bool getObject(SceneObjectId id, ResourceId& mesh, ResourceId& material,
                           glm::vec3& position, glm::vec3& rotation, glm::vec3& scale) const = 0;

    /**
    * \brief Set scene object parameters.
    */
    virtual void setObject(SceneObjectId id, ResourceId mesh, ResourceId material,
                           const glm::vec3& position, const glm::vec3& rotation,
                           const glm::vec3& scale) = 0;

    /**
     * \brief Creates point light in scene and returns id.
     */
    virtual SceneObjectId createPointLight(const glm::vec3& position, float radius,
                                           const glm::vec3& color, float intensity, bool castsShadow) = 0;

    /**
     * \brief Returns point light data.
     */
    virtual bool getPointLight(SceneObjectId id, glm::vec3& position, float& radius,
                               glm::vec3& color, float& intensity, bool& castsShadow) const = 0;

    /**
    * \brief Sets point light parameters.
    */
    virtual void setPointLight(SceneObjectId id, const glm::vec3& position, float radius,
                               const glm::vec3& color, float intensity, bool castsShadow) = 0;

	/**
	* \brief Creates directional light.
	*/
	virtual SceneObjectId createDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity, bool castsShadow) = 0;

	/**
	* \brief Returns directional light data.
	*/
	virtual bool getDirectionalLight(SceneObjectId id, glm::vec3& direction, glm::vec3& color, float& intensity, bool& castsShadow) const = 0;

	/**
	* \brief Sets directional light parameters.
	*/
	virtual void setDirectionalLight(SceneObjectId id, const glm::vec3& direction, const glm::vec3& color, float intensity, bool castsShadow) = 0;

	/**
	* \brief Sets the ambient light for the scene.
	*/
	virtual void setAmbientLight(const glm::vec3& color, float intensity) = 0;

	/**
	* \brief Returns the ambient light of the scene.
	*/
	virtual bool getAmbientLight(glm::vec3& color, float& intensity) const = 0;

    /**
     * \brief Creates scene query for specified camera.
     * Returns heap allocated query object. Control is transfered to the calling function.
     */
    virtual ISceneQuery* createQuery(const ICamera& camera) const = 0;
};

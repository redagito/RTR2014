#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "SceneConfig.h"
#include "ResourceConfig.h"

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
    * \brief Set object parameters.
    */
    virtual void setObject(ResourceId id, ResourceId mesh, ResourceId material,
                           const glm::vec3& position, const glm::vec3& rotation,
                           const glm::vec3& scale) = 0;

    /**
     * \brief Creates light in scene and returns id.
     */
    virtual SceneObjectId createLight(const glm::vec3& position, float radius,
                                      const glm::vec3& color) = 0;

    /**
     * \brief Returns light color in RGB format.
     */
    virtual bool getLight(SceneObjectId id, glm::vec3& position, float& radius,
                          glm::vec3& color) const = 0;

    /**
     * \brief Creates scene query for specified camera.
     * Returns heap allocated query object. Control is transfered to the calling function.
     */
    virtual ISceneQuery* createQuery(const ICamera& camera) const = 0;
};
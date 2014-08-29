#pragma once

#include <memory>
#include <cstdint>

#include <glm/glm.hpp>

#include "SceneConfig.h"
#include "ResourceConfig.h"

// Needed for unique_ptr
#include "ISceneQuery.h"

class ICamera;
class IResourceManager;

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
     * \brief Returns mesh id for object id.
     */
    virtual ResourceId getMesh(SceneObjectId object) const = 0;

    /**
     * \brief Returns material id for object id.
     */
    virtual ResourceId getMaterial(SceneObjectId object) const = 0;

    /**
     * \brief Returns position for the object id.
     * Return by value, not by reference to remain thread safe.
     */
    virtual glm::vec3 getObjectPosition(SceneObjectId object) const = 0;

    /**
     * \brief Returns rotation for the object id.
     * Return by value, not by reference to remain thread safe.
     */
    virtual glm::vec3 getObjectRotation(SceneObjectId object) const = 0;

    /**
     * \brief Returns scale for the object id.
     * Return by value, not by reference to remain thread safe.
     */
    virtual glm::vec3 getObjectScale(SceneObjectId object) const = 0;

    /**
     * \brief Creates light in scene and returns id.
     */
    virtual SceneObjectId createLight(const glm::vec3& position, float radius,
                                      const glm::vec3& color) = 0;

    /**
     * \brief Returns light color in RGB format.
     */
    virtual glm::vec3 getLightColor(SceneObjectId light) const = 0;

    /**
     * \brief Creates scene query for specified camera.
     */
    virtual std::unique_ptr<ISceneQuery> createQuery(const ICamera& camera) const = 0;
};

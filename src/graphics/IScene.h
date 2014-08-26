#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "resource/IResourceManager.h"

class ICamera;

/**
 * \brief Scene interface class.
 * The scene should only return internal ids. This provides the foundation for
 * future threading.
 */
class IScene
{
   public:
    typedef int ObjectId;
    typedef int LightId;

    typedef int ObjectQueryId;
    typedef int LightQueryId;

    virtual ~IScene();

    /**
     * \brief Creates object in scene and returns id.
     */
    virtual ObjectId createObject(IResourceManager::MeshId, IResourceManager::MaterialId,
                                  const glm::vec3& position, const glm::vec3& rotation,
                                  const glm::vec3& scale) = 0;

    /**
     * \brief Creates light in scene and returns id.
     */
    virtual LightId createLight(const glm::vec3& position, float radius,
                                const glm::vec3& color) = 0;

    /**
     * \brief Creates query id for objects in the scene, visible by the camera.
     */
    virtual ObjectQueryId queryVisibleObjects(const ICamera& camera) = 0;

    /**
     * \brief Creates query id for lights in the scene, visible by the camera.
     */
    virtual LightQueryId queryVisibleLights(const ICamera& camera) = 0;

    /**
     * \brief Returns next visible object for the provided id.
     */
    virtual ObjectId getNextObject(ObjectQueryId query) = 0;

    /**
     * \brief Returns next visible light for the provided id.
     */
    virtual ObjectId getNextLight(ObjectQueryId query) = 0;
};

#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "SceneConfig.h"
#include "resource/OrientedBoundingBox.h"
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
     *
     * The bounds are the extent of the mesh in mesh space, they are used for culling.
     */
    virtual SceneObjectId createObject(ResourceId mesh, ResourceId material,
                                       const glm::vec3& position, const glm::vec3& rotation,
                                       const glm::vec3& scale,
                                       const OrientedBoundingBox& localBounds,
                                       bool castsShadow = true) = 0;

    /**
     * \brief Returns scene object data.
     */
    virtual bool getObject(SceneObjectId id, ResourceId& mesh, ResourceId& material,
                           glm::vec3& position, glm::vec3& rotation, glm::vec3& scale,
                           bool& castsShadow) const = 0;

    /**
     * \brief Set scene object parameters.
     */
    virtual void setObject(ResourceId id, ResourceId mesh, ResourceId material,
                           const glm::vec3& position, const glm::vec3& rotation,
                           const glm::vec3& scale) = 0;

    /**
     * \brief Creates point light in scene and returns id.
     */
    virtual SceneObjectId createPointLight(const glm::vec3& position, float radius,
                                           const glm::vec3& color, float intensity,
                                           bool castsShadow) = 0;

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
    virtual SceneObjectId createDirectionalLight(const glm::vec3& direction, const glm::vec3& color,
                                                 float intensity, bool castsShadow) = 0;

    /**
     * \brief Returns directional light data.
     */
    virtual bool getDirectionalLight(SceneObjectId id, glm::vec3& direction, glm::vec3& color,
                                     float& intensity, bool& castsShadow) const = 0;

    /**
     * \brief Sets directional light parameters.
     */
    virtual void setDirectionalLight(SceneObjectId id, const glm::vec3& direction,
                                     const glm::vec3& color, float intensity, bool castsShadow) = 0;

    /**
     * \brief Sets the ambient light for the scene.
     */
    virtual void setAmbientLight(const glm::vec3& color, float intensity) = 0;

    /**
     * \brief Returns the ambient light of the scene.
     */
    virtual bool getAmbientLight(glm::vec3& color, float& intensity) const = 0;

    /**
     * \brief Returns the bounding sphere of the scene objects.
     *
     * Returns false if the scene has no objects.
     */
    virtual bool getSceneBounds(glm::vec3& center, float& radius) const = 0;

    /**
     * \brief Creates scene query for the given view projection.
     *
     * Objects outside the view frustum are not part of the query.
     */
    virtual std::unique_ptr<ISceneQuery> createQuery(const glm::mat4& viewProjection) const = 0;
};

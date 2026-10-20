#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "graphics/IScene.h"

struct SceneObject;
struct ScenePointLight;
struct SceneDirectionalLight;

/**
* \brief Simple scene implementation.
*/
class Scene : public IScene
{
   public:
    Scene();
    ~Scene();

    SceneObjectId createObject(ResourceId mesh, ResourceId material, const glm::vec3& position,
                               const glm::vec3& rotation, const glm::vec3& scale,
                               const OrientedBoundingBox& localBounds) override;

    bool getObject(SceneObjectId id, ResourceId& mesh, ResourceId& material, glm::vec3& position,
                   glm::vec3& rotation, glm::vec3& scale) const override;

    void setObject(ResourceId id, ResourceId mesh, ResourceId material, const glm::vec3& position,
                   const glm::vec3& rotation, const glm::vec3& scale) override;

    SceneObjectId createPointLight(const glm::vec3& position, float radius, const glm::vec3& color,
                                   float intensity, bool castsShadow) override;

    bool getPointLight(SceneObjectId id, glm::vec3& position, float& radius, glm::vec3& color,
                       float& intensity, bool& castsShadow) const override;

    void setPointLight(SceneObjectId id, const glm::vec3& position, float radius,
                       const glm::vec3& color, float intensity, bool castsShadow) override;

    SceneObjectId createDirectionalLight(const glm::vec3& direction, const glm::vec3& color,
                                         float intensity, bool castsShadow) override;

    bool getDirectionalLight(SceneObjectId id, glm::vec3& direction, glm::vec3& color,
                             float& intensity, bool& castsShadow) const override;

    void setDirectionalLight(SceneObjectId id, const glm::vec3& direction, const glm::vec3& color,
                             float intensity, bool castsShadow) override;

    void setAmbientLight(const glm::vec3& color, float intensity) override;

    bool getAmbientLight(glm::vec3& color, float& intensity) const override;

    ISceneQuery* createQuery(const glm::mat4& viewProjection) const override;

   private:
    glm::vec3 m_ambientColor; /**< Global ambient light color. */
    float m_ambientIntensity; /**< Global ambient light intensity. */

    std::vector<SceneObject> m_objects;                     /**< Drawable scene objects. */
    std::vector<ScenePointLight> m_pointLights;             /**< Point lights. */
    std::vector<SceneDirectionalLight> m_directionalLights; /**< Directional lights. */
};

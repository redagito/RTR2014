#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "graphics/IScene.h"

struct SSceneObject;
struct SScenePointLight;
struct SSceneDirectionalLight;

/**
* \brief Simple scene implementation.
*/
class CScene : public IScene
{
   public:
    CScene();
    ~CScene();

    SceneObjectId createObject(ResourceId mesh, ResourceId material, const glm::vec3& position,
                               const glm::vec3& rotation, const glm::vec3& scale);

    bool getObject(SceneObjectId id, ResourceId& mesh, ResourceId& material, glm::vec3& position,
                   glm::vec3& rotation, glm::vec3& scale) const;

    void setObject(ResourceId id, ResourceId mesh, ResourceId material, const glm::vec3& position,
                   const glm::vec3& rotation, const glm::vec3& scale);

    SceneObjectId createPointLight(const glm::vec3& position, float radius, const glm::vec3& color,
                                   float intensity);

    bool getPointLight(SceneObjectId id, glm::vec3& position, float& radius, glm::vec3& color,
                       float& intensity) const;

    void setPointLight(SceneObjectId id, const glm::vec3& position, float radius,
                       const glm::vec3& color, float intensity);

    SceneObjectId createDirectionalLight(const glm::vec3& direction, const glm::vec3& color,
                                         float intensity);

    bool getDirectionalLight(SceneObjectId id, glm::vec3& direction, glm::vec3& color,
                             float& intensity) const;

    void setDirectionalLight(SceneObjectId id, const glm::vec3& direction, const glm::vec3& color,
                             float intensity);

    void setAmbientLight(const glm::vec3& color, float intensity);

    bool getAmbientLight(glm::vec3& color, float& intensity) const;

    ISceneQuery* createQuery(const ICamera& camera) const;

   private:
    glm::vec3 m_ambientColor; /**< Global ambient light color. */
    float m_ambientIntensity; /**< Global ambient light intensity. */

    std::vector<SSceneObject> m_objects;                     /**< Drawable scene objects. */
    std::vector<SScenePointLight> m_pointLights;             /**< Point lights. */
    std::vector<SSceneDirectionalLight> m_directionalLights; /**< Directional lights. */
};

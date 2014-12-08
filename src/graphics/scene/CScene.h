#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "graphics/IScene.h"

struct SSceneObject;
struct SScenePointLight;

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

    SceneObjectId createPointLight(const glm::vec3& position, float radius, const glm::vec3& color, float intensity);

	bool getPointLight(SceneObjectId id, glm::vec3& position, float& radius, glm::vec3& color, float& intensity) const;

	void setPointLight(SceneObjectId id, const glm::vec3& position, float radius, const glm::vec3& color, float intensity);

    ISceneQuery* createQuery(const ICamera& camera) const;

   private:
    std::vector<SSceneObject> m_objects;
    std::vector<SScenePointLight> m_pointLights;
};

#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "graphics/IScene.h"

struct SSceneObject;
struct SSceneLight;

class CScene : public IScene
{
   public:
    CScene();
    ~CScene();

    SceneObjectId createObject(ResourceId mesh, ResourceId material, const glm::vec3& position,
                               const glm::vec3& rotation, const glm::vec3& scale);

    bool getObject(SceneObjectId id, ResourceId& mesh, ResourceId& material, glm::vec3& position,
                   glm::vec3& rotation, glm::vec3& scale) const;

    SceneObjectId createLight(const glm::vec3& position, float radius, const glm::vec3& color);

    bool getLight(SceneObjectId id, glm::vec3& position, float& radius, glm::vec3& color) const;

    std::unique_ptr<ISceneQuery> createQuery(const ICamera& camera) const;

   private:
    std::vector<SSceneObject> m_objects;
    std::vector<SSceneLight> m_lights;
};

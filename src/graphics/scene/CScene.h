#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "graphics/IScene.h"

class CSceneObject;

class CScene : public IScene
{
   public:
    CScene();
    ~CScene();

    void addObject(std::shared_ptr<CSceneObject> object);
    const std::vector<std::shared_ptr<CSceneObject>>& getObjects();

    SceneObjectId createObject(ResourceId, ResourceId, const glm::vec3& position,
                               const glm::vec3& rotation, const glm::vec3& scale);

    ResourceId getMesh(SceneObjectId object) const;
    ResourceId getMaterial(SceneObjectId object) const;

    glm::vec3 getObjectPosition(SceneObjectId object) const;
    glm::vec3 getObjectRotation(SceneObjectId object) const;
    glm::vec3 getObjectScale(SceneObjectId object) const;

    SceneObjectId createLight(const glm::vec3& position, float radius, const glm::vec3& color);
    glm::vec3 getLightColor(SceneObjectId light) const;

    std::unique_ptr<ISceneQuery> createQuery(const ICamera& camera) const;

   private:
    std::vector<std::shared_ptr<CSceneObject>> m_objects;
};

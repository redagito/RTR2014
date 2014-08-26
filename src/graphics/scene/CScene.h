#pragma once

#include <vector>
#include <memory>

#include "graphics/IScene.h"

class CSceneObject;

class CScene : public IScene
{
   public:
    CScene();
    ~CScene();

    void addObject(std::shared_ptr<CSceneObject> object);
    const std::vector<std::shared_ptr<CSceneObject>>& getObjects();

    ObjectId createObject(IResourceManager::MeshId, IResourceManager::MaterialId,
                          const glm::vec3& position, const glm::vec3& rotation,
                          const glm::vec3& scale);
    LightId createLight(const glm::vec3& position, float radius, const glm::vec3& color);

    ObjectQueryId queryVisibleObjects(const ICamera& camera);
    LightQueryId queryVisibleLights(const ICamera& camera);

    ObjectId getNextObject(ObjectQueryId query);
    ObjectId getNextLight(ObjectQueryId query);

   private:
    std::vector<std::shared_ptr<CSceneObject>> m_objects;
};

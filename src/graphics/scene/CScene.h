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

    ObjectId createObject(IResourceManager::ResourceId, IResourceManager::ResourceId,
                          const glm::vec3& position, const glm::vec3& rotation,
                          const glm::vec3& scale);

    IResourceManager::ResourceId getMesh(ObjectId object) const;
    IResourceManager::ResourceId getMaterial(ObjectId object) const;

    glm::vec3 getPosition(ObjectId object) const;
    glm::vec3 getRotation(ObjectId object) const;
    glm::vec3 getScale(ObjectId object) const;

    LightId createLight(const glm::vec3& position, float radius, const glm::vec3& color);

    ObjectQueryId queryVisibleObjects(const ICamera& camera) const;
    LightQueryId queryVisibleLights(const ICamera& camera) const;

    ObjectId getNextObject(ObjectQueryId query) const;
    ObjectId getNextLight(ObjectQueryId query) const;

   private:
    std::vector<std::shared_ptr<CSceneObject>> m_objects;
};

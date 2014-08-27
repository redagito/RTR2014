#include "CScene.h"

CScene::CScene() {}

CScene::~CScene() {}

void CScene::addObject(std::shared_ptr<CSceneObject> object) { m_objects.push_back(object); }

const std::vector<std::shared_ptr<CSceneObject>>& CScene::getObjects() { return m_objects; }

IScene::ObjectId CScene::createObject(IResourceManager::ResourceId, IResourceManager::ResourceId,
                                      const glm::vec3& position, const glm::vec3& rotation,
                                      const glm::vec3& scale)
{
    // TODO Implement
    return -1;
}

IResourceManager::ResourceId CScene::getMesh(IScene::ObjectId object) const
{
    // TODO Implement
    return -1;
}

IResourceManager::ResourceId CScene::getMaterial(IScene::ObjectId object) const
{
    // TODO Implement
    return -1;
}

glm::vec3 CScene::getPosition(IScene::ObjectId object) const
{
    // TODO Implement
    return glm::vec3(0.f);
}

glm::vec3 CScene::getRotation(IScene::ObjectId object) const
{
    // TODO Implement
    return glm::vec3(0.f);
}

glm::vec3 CScene::getScale(IScene::ObjectId object) const
{
    // TODO Implement
    return glm::vec3(0.f);
}

IScene::LightId CScene::createLight(const glm::vec3& position, float radius, const glm::vec3& color)
{
    // TODO Implement
    return -1;
}

IScene::ObjectQueryId CScene::queryVisibleObjects(const ICamera& camera) const
{
    // TODO Implement
    return -1;
}

IScene::LightQueryId CScene::queryVisibleLights(const ICamera& camera) const
{
    // TODO Implement
    return -1;
}

IScene::ObjectId CScene::getNextObject(IScene::ObjectQueryId query) const
{
    // TODO Implement
    return -1;
}

IScene::ObjectId CScene::getNextLight(IScene::ObjectQueryId query) const
{
    // TODO Implement
    return -1;
}

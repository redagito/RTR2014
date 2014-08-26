#include "CScene.h"

CScene::CScene() {}

CScene::~CScene() {}

void CScene::addObject(std::shared_ptr<CSceneObject> object) { m_objects.push_back(object); }

const std::vector<std::shared_ptr<CSceneObject>>& CScene::getObjects() { return m_objects; }

IScene::ObjectId CScene::createObject(IResourceManager::MeshId, IResourceManager::MaterialId,
                                      const glm::vec3& position, const glm::vec3& rotation,
                                      const glm::vec3& scale)
{
    return -1;
}

IScene::LightId CScene::createLight(const glm::vec3& position, float radius, const glm::vec3& color)
{
    return -1;
}

IScene::ObjectQueryId CScene::queryVisibleObjects(const ICamera& camera) { return -1; }

IScene::LightQueryId CScene::queryVisibleLights(const ICamera& camera) { return -1; }

IScene::ObjectId CScene::getNextObject(IScene::ObjectQueryId query) { return -1; }

IScene::ObjectId CScene::getNextLight(IScene::ObjectQueryId query) { return -1; }

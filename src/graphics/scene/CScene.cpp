#include "CScene.h"

#include "CSceneQuery.h"

CScene::CScene() {}

CScene::~CScene() {}

void CScene::addObject(std::shared_ptr<CSceneObject> object) { m_objects.push_back(object); }

const std::vector<std::shared_ptr<CSceneObject>>& CScene::getObjects() { return m_objects; }

SceneObjectId CScene::createObject(ResourceId mesh, ResourceId material, const glm::vec3& position,
                                   const glm::vec3& rotation, const glm::vec3& scale)
{
    // TODO Implement
    return -1;
}

ResourceId CScene::getMesh(SceneObjectId object) const
{
    // TODO Implement
    return -1;
}

ResourceId CScene::getMaterial(SceneObjectId object) const
{
    // TODO Implement
    return -1;
}

glm::vec3 CScene::getObjectPosition(SceneObjectId object) const
{
    // TODO Implement
    return glm::vec3(0.f);
}

glm::vec3 CScene::getObjectRotation(SceneObjectId object) const
{
    // TODO Implement
    return glm::vec3(0.f);
}

glm::vec3 CScene::getObjectScale(SceneObjectId object) const
{
    // TODO Implement
    return glm::vec3(0.f);
}

SceneObjectId CScene::createLight(const glm::vec3& position, float radius, const glm::vec3& color)
{
    // TODO Implement
    return -1;
}

glm::vec3 CScene::getLightColor(SceneObjectId light) const
{
    // TODO Implement
    return glm::vec3(0.f);
}

std::unique_ptr<ISceneQuery> CScene::createQuery(const ICamera& camera) const
{
    // New query
    CSceneQuery* query = new CSceneQuery;

    // TODO Add visible objects

    // Return query
    return std::unique_ptr<ISceneQuery>(query);
}

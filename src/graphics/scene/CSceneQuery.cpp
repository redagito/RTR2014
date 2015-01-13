#include "CSceneQuery.h"

#include "graphics/scene/CScene.h"
#include "graphics/ICamera.h"

CSceneQuery::CSceneQuery(unsigned int objectStorage, unsigned int lightStorage)
{
    m_visibleObjects.reserve(objectStorage);
    m_visiblePointLights.reserve(lightStorage);
}

bool CSceneQuery::hasNextObject() const { return m_nextObjectIndex < m_visibleObjects.size(); }

SceneObjectId CSceneQuery::getNextObject()
{
    if (!hasNextObject())
    {
        // Invalid id
        // TODO Remove magic number
        return -1;
    }
    // Get id
    SceneObjectId id = m_visibleObjects.at(m_nextObjectIndex);
    // Advance index
    ++m_nextObjectIndex;
    return id;
}

bool CSceneQuery::hasNextPointLight() const
{
    return m_nextPointLightIndex < m_visiblePointLights.size();
}

SceneObjectId CSceneQuery::getNextPointLight()
{
    if (!hasNextPointLight())
    {
        // Invalid id
        // TODO Remove magic number
        return -1;
    }
    // Get id
    SceneObjectId id = m_visiblePointLights.at(m_nextPointLightIndex);
    // Advance index
    ++m_nextPointLightIndex;
    return id;
}

bool CSceneQuery::hasNextDirectionalLight() const
{
    return m_nextDirectionalLightIndex < m_visibleDirectionalLights.size();
}

SceneObjectId CSceneQuery::getNextDirectionalLight()
{
    if (!hasNextDirectionalLight())
    {
        // Invalid object id
        return invalidObject;
    }
    // Get id
    SceneObjectId id = m_visibleDirectionalLights.at(m_nextDirectionalLightIndex);
    // Advance index
    ++m_nextDirectionalLightIndex;
    return id;
}

void CSceneQuery::addObject(SceneObjectId id) { m_visibleObjects.push_back(id); }

void CSceneQuery::addPointLight(SceneObjectId id) { m_visiblePointLights.push_back(id); }

void CSceneQuery::addDirectionalLight(SceneObjectId id)
{
    m_visibleDirectionalLights.push_back(id);
}

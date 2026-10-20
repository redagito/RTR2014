#include "graphics/scene/SceneQuery.h"

#include "graphics/ICamera.h"
#include "graphics/scene/Scene.h"

SceneQuery::SceneQuery(unsigned int objectStorage, unsigned int lightStorage)
{
    m_visibleObjects.reserve(objectStorage);
    m_visiblePointLights.reserve(lightStorage);
    m_visibleDirectionalLights.reserve(lightStorage);
}

bool SceneQuery::hasNextObject() const { return m_nextObjectIndex < m_visibleObjects.size(); }

SceneObjectId SceneQuery::getNextObject()
{
    if (!hasNextObject())
    {
        // Invalid id
        return invalidObject;
    }
    // Get id
    SceneObjectId id = m_visibleObjects.at(m_nextObjectIndex);
    // Advance index
    ++m_nextObjectIndex;
    return id;
}

bool SceneQuery::hasNextPointLight() const
{
    return m_nextPointLightIndex < m_visiblePointLights.size();
}

SceneObjectId SceneQuery::getNextPointLight()
{
    if (!hasNextPointLight())
    {
        // Invalid id
        return invalidObject;
    }
    // Get id
    SceneObjectId id = m_visiblePointLights.at(m_nextPointLightIndex);
    // Advance index
    ++m_nextPointLightIndex;
    return id;
}

bool SceneQuery::hasNextDirectionalLight() const
{
    return m_nextDirectionalLightIndex < m_visibleDirectionalLights.size();
}

SceneObjectId SceneQuery::getNextDirectionalLight()
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

void SceneQuery::addObject(SceneObjectId id) { m_visibleObjects.push_back(id); }

void SceneQuery::addPointLight(SceneObjectId id) { m_visiblePointLights.push_back(id); }

void SceneQuery::addDirectionalLight(SceneObjectId id) { m_visibleDirectionalLights.push_back(id); }

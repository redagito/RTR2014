#include "CSceneQuery.h"

#include "graphics/scene/CScene.h"
#include "graphics/ICamera.h"

CSceneQuery::CSceneQuery(unsigned int objectStorage, unsigned int lightStorage)
    : m_nextObjectIndex(0)
{
    m_visibleObjects.reserve(objectStorage);
    m_visibleLights.reserve(lightStorage);
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

void CSceneQuery::addObject(SceneObjectId id) { m_visibleObjects.push_back(id); }

void CSceneQuery::addLight(SceneObjectId id) { m_visibleLights.push_back(id); }

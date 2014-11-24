#pragma once

#include <vector>

#include "graphics/ISceneQuery.h"

/**
 * \brief Scene query implementation.
 */
class CSceneQuery : public ISceneQuery
{
   public:
    /**
     * \brief Sets initial storage.
     */
    CSceneQuery(unsigned int objectStorage = 200, unsigned int lightStorage = 100);

    bool hasNextObject() const;

    SceneObjectId getNextObject();

    bool hasNextPointLight() const;

    SceneObjectId getNextPointLight();

    /**
     * \brief Adds object id.
     */
    void addObject(SceneObjectId id);

    /**
     * \brief Adds light id.
     */
    void addPointLight(SceneObjectId id);

   private:
    unsigned int m_nextObjectIndex = 0;
    unsigned int m_nextPointLightIndex = 0;
    std::vector<SceneObjectId> m_visibleObjects;     /**< Visible objects. */
    std::vector<SceneObjectId> m_visiblePointLights; /**< Visible lights. */
};

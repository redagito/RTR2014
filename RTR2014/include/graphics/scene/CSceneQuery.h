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

    bool hasNextDirectionalLight() const;

    SceneObjectId getNextDirectionalLight();

    /**
     * \brief Adds object id.
     */
    void addObject(SceneObjectId id);

    /**
     * \brief Adds point light id.
     */
    void addPointLight(SceneObjectId id);

    /**
    * \brief Adds directional light id.
    */
    void addDirectionalLight(SceneObjectId id);

   private:
    unsigned int m_nextObjectIndex = 0;
    unsigned int m_nextPointLightIndex = 0;
    unsigned int m_nextDirectionalLightIndex = 0;
    std::vector<SceneObjectId> m_visibleObjects;           /**< Visible objects. */
    std::vector<SceneObjectId> m_visiblePointLights;       /**< Visible lights. */
    std::vector<SceneObjectId> m_visibleDirectionalLights; /**< Visible directional lights. */
};

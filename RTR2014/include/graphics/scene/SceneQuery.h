#pragma once

#include <vector>

#include "graphics/ISceneQuery.h"

/**
 * \brief Scene query implementation.
 */
class SceneQuery : public ISceneQuery
{
   public:
    /**
     * \brief Sets initial storage.
     */
    SceneQuery(unsigned int objectStorage = 200, unsigned int lightStorage = 100);

    bool hasNextObject() const override;

    SceneObjectId getNextObject() override;

    bool hasNextPointLight() const override;

    SceneObjectId getNextPointLight() override;

    bool hasNextDirectionalLight() const override;

    SceneObjectId getNextDirectionalLight() override;

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

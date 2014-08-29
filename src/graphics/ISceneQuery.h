#pragma once

#include "SceneConfig.h"

/**
 * \brief Scene query interface class.
 * Provides a spatial query interface for scene objects.
 */
class ISceneQuery
{
   public:
    /**
     * \brief Virtual destructor for interface class.
     */
    virtual ~ISceneQuery();

    /**
     * \brief Has visible objects to return.
     */
    virtual bool hasNextObject() const = 0;

    /**
     * \brief Returns id of next visible object.
     */
    virtual SceneObjectId getNextObject() = 0;
};

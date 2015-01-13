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

    /**
     * \brief Has visible point lights to return.
     */
    virtual bool hasNextPointLight() const = 0;

    /**
    * \brief Returns id of next visible point light.
    */
    virtual SceneObjectId getNextPointLight() = 0;

    /**
    * \brief Has visible directional light to return.
    */
    virtual bool hasNextDirectionalLight() const = 0;

    /**
    * \brief Returns id of next visible directional light.
    */
    virtual SceneObjectId getNextDirectionalLight() = 0;
};

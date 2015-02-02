#pragma once

/**
* \brief Animation controller interface class.
*
* An animation controller is attached to a scene object and
* performs a time based state change (animation) on that object.
*/
class IAnimationController
{
   public:
    /**
    * \brief Virtual detsructor for interface class.
    */
    virtual ~IAnimationController();

    /**
    * \brief Performs animation update on controlled object.
    */
    virtual void update(float timeStep) = 0;
};
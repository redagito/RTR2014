#pragma once

class IAnimationController
{
   public:
    virtual ~IAnimationController();

	/**
	* \brief Performs animation update on controlled object.
	*/
    virtual void update(float timeStep) = 0;
};
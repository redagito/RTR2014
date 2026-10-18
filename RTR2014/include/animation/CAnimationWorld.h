#pragma once

#include <memory>
#include <list>

class IAnimationController;

/**
* \brief The animation world stores a collection of animation controller.
*/
class CAnimationWorld
{
public:
	/**
	* \brief Adds new animation controller to the world.
	*/
	void addAnimationController(const std::shared_ptr<IAnimationController>& controller);

	/**
	* \brief Update all controller.
	*/
	void update(float timeDelta);

private:
	std::list<std::shared_ptr<IAnimationController>> m_animationController; /**< Registered animation controller. */
};
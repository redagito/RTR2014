#include <cassert>

#include "animation/AnimationWorld.h"
#include "animation/IAnimationController.h"

void AnimationWorld::addAnimationController(const std::shared_ptr<IAnimationController>& controller)
{
	assert(controller != nullptr);
	if (controller != nullptr)
	{
		m_animationController.push_back(controller);
	}
}

void AnimationWorld::update(float timeDiff)
{
	for (auto& controller : m_animationController)
	{
		controller->update(timeDiff);
	}
	return;
}

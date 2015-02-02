#include <cassert>

#include "CAnimationWorld.h"
#include "IAnimationController.h"

void CAnimationWorld::addAnimationController(const std::shared_ptr<IAnimationController>& controller)
{
	assert(controller != nullptr);
	if (controller != nullptr)
	{
		m_animationController.push_back(controller);
	}
}

void CAnimationWorld::update(float timeDiff)
{
	for (auto& controller : m_animationController)
	{
		controller->update(timeDiff);
	}
	return;
}
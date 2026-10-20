#pragma once

#include <list>
#include <memory>

class IAnimationController;

/**
 * \brief The animation world stores a collection of animation controller.
 */
class AnimationWorld
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
    std::list<std::shared_ptr<IAnimationController>>
        m_animationController; /**< Registered animation controller. */
};

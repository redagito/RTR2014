#pragma once

#include <glm/glm.hpp>

#include "Animation.h"
#include "IAnimationController.h"
#include "graphics/SceneConfig.h"

class IScene;

class CMovementController : public IAnimationController
{
   public:
    CMovementController(SceneObjectId id, AnimationObjectType type, IScene& scene,
                        const glm::vec3& direction);

    void update(float timeStep);

   private:
    SceneObjectId m_objectId = -1;
    AnimationObjectType m_type;
    IScene& m_scene;

    glm::vec3 m_direction;
    float m_endTime;

    float m_currentTimePoint = 0.0f;
};
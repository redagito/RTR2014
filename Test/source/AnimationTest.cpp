#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <vector>

#include <glm/glm.hpp>

#include "animation/MovementController.h"
#include "animation/RotationController.h"
#include "graphics/scene/Scene.h"
#include "resource/OrientedBoundingBox.h"

namespace
{
std::vector<glm::vec3> boxPoints(const glm::vec3& min, const glm::vec3& max)
{
    std::vector<glm::vec3> points;
    for (int x = 0; x < 2; ++x)
    {
        for (int y = 0; y < 2; ++y)
        {
            for (int z = 0; z < 2; ++z)
            {
                points.push_back(glm::vec3(x ? max.x : min.x, y ? max.y : min.y,
                                           z ? max.z : min.z));
            }
        }
    }
    return points;
}

SceneObjectId createObject(Scene& scene, const glm::vec3& position)
{
    const auto bounds = OrientedBoundingBox::fromPoints(boxPoints(glm::vec3(0.f), glm::vec3(1.f)));
    return scene.createObject(-1, -1, position, glm::vec3(0.f), glm::vec3(1.f), bounds);
}

glm::vec3 objectPosition(const Scene& scene, SceneObjectId id)
{
    ResourceId mesh = 0;
    ResourceId material = 0;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    REQUIRE(scene.getObject(id, mesh, material, position, rotation, scale));
    return position;
}
} // namespace

TEST_CASE("the movement controller returns the object to its position", "[animation]")
{
    Scene scene;
    const SceneObjectId id = createObject(scene, glm::vec3(0.f));

    // The direction is four units long, so a cycle takes eight seconds.
    MovementController controller(id, AnimationObjectType::Model, scene, glm::vec3(0.f, 0.f, 4.f));
    for (int i = 0; i < 100; ++i)
    {
        controller.update(0.4f);
    }

    // Five whole cycles have passed, the object is back where it started.
    const glm::vec3 position = objectPosition(scene, id);
    REQUIRE(position.x == Catch::Approx(0.f).margin(0.0001f));
    REQUIRE(position.y == Catch::Approx(0.f).margin(0.0001f));
    REQUIRE(position.z == Catch::Approx(0.f).margin(0.0001f));
}

TEST_CASE("the movement controller moves the object along its direction", "[animation]")
{
    Scene scene;
    const SceneObjectId id = createObject(scene, glm::vec3(0.f));

    MovementController controller(id, AnimationObjectType::Model, scene, glm::vec3(0.f, 0.f, 4.f));
    controller.update(1.f);

    // The speed is one unit per second.
    REQUIRE(objectPosition(scene, id).z == Catch::Approx(1.f));
}

TEST_CASE("the rotation controller keeps the angles in range", "[animation]")
{
    Scene scene;
    const SceneObjectId id = createObject(scene, glm::vec3(0.f));

    RotationController controller(id, AnimationObjectType::Model, scene, glm::vec3(1.f, 2.f, 0.f));
    for (int i = 0; i < 10000; ++i)
    {
        controller.update(0.1f);
    }

    ResourceId mesh = 0;
    ResourceId material = 0;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    REQUIRE(scene.getObject(id, mesh, material, position, rotation, scale));

    const float fullTurn = 6.28318530717958647692f;
    REQUIRE(rotation.x >= 0.f);
    REQUIRE(rotation.x < fullTurn);
    REQUIRE(rotation.y >= 0.f);
    REQUIRE(rotation.y < fullTurn);
}

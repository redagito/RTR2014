#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/Frustum.h"
#include "resource/OrientedBoundingBox.h"
#include "graphics/scene/Scene.h"
#include "graphics/scene/SceneQuery.h"

namespace
{
std::vector<glm::vec3> boxPoints(const glm::vec3& center, const glm::vec3& halfExtents)
{
    std::vector<glm::vec3> points;
    for (int i = 0; i < 8; ++i)
    {
        points.emplace_back(center.x + ((i & 1) ? halfExtents.x : -halfExtents.x),
                            center.y + ((i & 2) ? halfExtents.y : -halfExtents.y),
                            center.z + ((i & 4) ? halfExtents.z : -halfExtents.z));
    }
    return points;
}

bool contains(const OrientedBoundingBox& box, const glm::vec3& point)
{
    glm::vec3 delta = point - box.getCenter();
    for (int i = 0; i < 3; ++i)
    {
        glm::vec3 axis = glm::vec3(box.getAxes()[i]);
        if (std::fabs(glm::dot(delta, axis)) > box.getHalfExtents()[i] + 0.0001f)
        {
            return false;
        }
    }
    return true;
}

bool approxEqual(const glm::vec3& a, const glm::vec3& b, float margin)
{
    return std::fabs(a.x - b.x) <= margin && std::fabs(a.y - b.y) <= margin &&
           std::fabs(a.z - b.z) <= margin;
}

// The axes of the box are sorted by descending extent, so the extents are compared as a set.
glm::vec3 sorted(const glm::vec3& v)
{
    glm::vec3 result = v;
    if (result.x < result.y)
    {
        result.x = result.y;
        result.y = v.x;
    }
    if (result.y < result.z)
    {
        std::swap(result.y, result.z);
    }
    if (result.x < result.y)
    {
        std::swap(result.x, result.y);
    }
    return result;
}
} // namespace

TEST_CASE("OrientedBoundingBox of an axis aligned box matches its extents", "[bounding]")
{
    auto points = boxPoints(glm::vec3(1.f, 2.f, 3.f), glm::vec3(0.5f, 1.f, 2.f));

    auto box = OrientedBoundingBox::fromPoints(points);

    REQUIRE(approxEqual(box.getCenter(), glm::vec3(1.f, 2.f, 3.f), 0.001f));
    REQUIRE(approxEqual(sorted(box.getHalfExtents()), glm::vec3(2.f, 1.f, 0.5f), 0.001f));
}

TEST_CASE("OrientedBoundingBox contains every point it was computed from", "[bounding]")
{
    std::vector<glm::vec3> points;
    for (int i = 0; i < 32; ++i)
    {
        float angle = static_cast<float>(i) * 0.37f;
        points.emplace_back(std::cos(angle) * 3.f, std::sin(angle) * 1.5f,
                            std::sin(angle * 2.f) * 2.f);
    }

    auto box = OrientedBoundingBox::fromPoints(points);

    for (const glm::vec3& point : points)
    {
        REQUIRE(contains(box, point));
    }
}

TEST_CASE("OrientedBoundingBox aligns its axes with a rotated box", "[bounding]")
{
    // A box that is long in x and thin in y and z, rotated around the z axis
    glm::mat4 rotation = glm::rotate(glm::mat4(1.f), glm::radians(45.f), glm::vec3(0.f, 0.f, 1.f));
    std::vector<glm::vec3> points;
    for (const glm::vec3& point : boxPoints(glm::vec3(0.f), glm::vec3(4.f, 0.5f, 0.5f)))
    {
        points.emplace_back(glm::vec3(rotation * glm::vec4(point, 1.f)));
    }

    auto box = OrientedBoundingBox::fromPoints(points);

    // The principal axis is the long one, its half extent is the length of the diagonal
    float longest = box.getHalfExtents()[0];
    float shortest = box.getHalfExtents()[1];
    REQUIRE(longest == Catch::Approx(4.f).margin(0.01f));
    REQUIRE(shortest == Catch::Approx(0.5f).margin(0.01f));
}

TEST_CASE("OrientedBoundingBox transform moves and scales the box", "[bounding]")
{
    auto box = OrientedBoundingBox::fromPoints(boxPoints(glm::vec3(0.f), glm::vec3(1.f, 2.f, 3.f)));

    glm::mat4 matrix = glm::translate(glm::mat4(1.f), glm::vec3(10.f, 0.f, 0.f)) *
                       glm::scale(glm::mat4(1.f), glm::vec3(2.f, 2.f, 2.f));
    auto transformed = box.transformed(matrix);

    REQUIRE(approxEqual(transformed.getCenter(), glm::vec3(10.f, 0.f, 0.f), 0.001f));
    REQUIRE(approxEqual(sorted(transformed.getHalfExtents()), glm::vec3(6.f, 4.f, 2.f), 0.001f));
}

TEST_CASE("Frustum keeps a box in view and rejects a box behind the camera", "[bounding]")
{
    glm::mat4 view = glm::lookAt(glm::vec3(0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 projection = glm::perspective(glm::radians(60.f), 1.33f, 0.1f, 100.f);
    Frustum frustum(projection * view);

    auto inFront = OrientedBoundingBox::fromPoints(boxPoints(glm::vec3(0.f, 0.f, -10.f),
                                                             glm::vec3(1.f)));
    auto behind = OrientedBoundingBox::fromPoints(boxPoints(glm::vec3(0.f, 0.f, 10.f),
                                                            glm::vec3(1.f)));

    REQUIRE(frustum.intersects(inFront));
    REQUIRE_FALSE(frustum.intersects(behind));
}

TEST_CASE("Frustum rejects a box that is off to the side", "[bounding]")
{
    glm::mat4 view = glm::lookAt(glm::vec3(0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 projection = glm::perspective(glm::radians(60.f), 1.33f, 0.1f, 100.f);
    Frustum frustum(projection * view);

    auto inFront = OrientedBoundingBox::fromPoints(boxPoints(glm::vec3(0.f, 0.f, -10.f),
                                                             glm::vec3(1.f)));
    auto toTheSide = OrientedBoundingBox::fromPoints(boxPoints(glm::vec3(50.f, 0.f, -10.f),
                                                               glm::vec3(1.f)));

    REQUIRE(frustum.intersects(inFront));
    REQUIRE_FALSE(frustum.intersects(toTheSide));
}

TEST_CASE("Scene query only returns objects inside the view frustum", "[scene]")
{
    Scene scene;
    glm::mat4 view = glm::lookAt(glm::vec3(0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 projection = glm::perspective(glm::radians(60.f), 1.33f, 0.1f, 100.f);

    auto bounds = OrientedBoundingBox::fromPoints(boxPoints(glm::vec3(0.f), glm::vec3(1.f)));
    scene.createObject(-1, -1, glm::vec3(0.f, 0.f, -10.f), glm::vec3(0.f), glm::vec3(1.f), bounds);
    scene.createObject(-1, -1, glm::vec3(50.f, 0.f, -10.f), glm::vec3(0.f), glm::vec3(1.f), bounds);

    std::unique_ptr<ISceneQuery> query(scene.createQuery(projection * view));

    REQUIRE(query->hasNextObject());
    SceneObjectId id = query->getNextObject();
    REQUIRE(id == 0);
    REQUIRE_FALSE(query->hasNextObject());
}

TEST_CASE("the scene bounds hold every object", "[scene]")
{
    Scene scene;
    auto bounds = OrientedBoundingBox::fromPoints(boxPoints(glm::vec3(0.f), glm::vec3(1.f)));

    scene.createObject(-1, -1, glm::vec3(-30.f, 0.f, -10.f), glm::vec3(0.f), glm::vec3(1.f),
                       bounds);
    scene.createObject(-1, -1, glm::vec3(30.f, 0.f, -10.f), glm::vec3(0.f), glm::vec3(1.f),
                       bounds);

    glm::vec3 center;
    float radius = 0.f;
    REQUIRE(scene.getSceneBounds(center, radius));

    // Every object has to be inside the sphere around the center.
    REQUIRE(glm::length(glm::vec3(-30.f, 0.f, -10.f) - center) <= radius);
    REQUIRE(glm::length(glm::vec3(30.f, 0.f, -10.f) - center) <= radius);
    // The object itself has to fit in it as well.
    REQUIRE(radius >= glm::length(bounds.getHalfExtents()));
}

TEST_CASE("a scene without objects has no bounds", "[scene]")
{
    Scene scene;

    glm::vec3 center;
    float radius = 0.f;
    REQUIRE_FALSE(scene.getSceneBounds(center, radius));
}

TEST_CASE("the scene bounds leave out the objects that cast no shadow", "[scene]")
{
    Scene scene;
    auto bounds = OrientedBoundingBox::fromPoints(boxPoints(glm::vec3(0.f), glm::vec3(1.f)));

    scene.createObject(-1, -1, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f), glm::vec3(1.f), bounds);
    // A big object that does not cast a shadow must not enlarge the volume.
    scene.createObject(-1, -1, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f), glm::vec3(1000.f), bounds,
                       false);

    glm::vec3 center;
    float radius = 0.f;
    REQUIRE(scene.getSceneBounds(center, radius));
    REQUIRE(radius < 10.f);
}

TEST_CASE("Scene query follows an object that moves", "[scene]"){
    Scene scene;
    glm::mat4 view = glm::lookAt(glm::vec3(0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 projection = glm::perspective(glm::radians(60.f), 1.33f, 0.1f, 100.f);

    auto bounds = OrientedBoundingBox::fromPoints(boxPoints(glm::vec3(0.f), glm::vec3(1.f)));
    SceneObjectId id = scene.createObject(-1, -1, glm::vec3(0.f, 0.f, -10.f), glm::vec3(0.f),
                                          glm::vec3(1.f), bounds);

    std::unique_ptr<ISceneQuery> visible(scene.createQuery(projection * view));
    REQUIRE(visible->hasNextObject());
    visible->getNextObject();
    REQUIRE_FALSE(visible->hasNextObject());

    // Move the object out of the view
    scene.setObject(id, -1, -1, glm::vec3(50.f, 0.f, -10.f), glm::vec3(0.f), glm::vec3(1.f));

    std::unique_ptr<ISceneQuery> culled(scene.createQuery(projection * view));
    REQUIRE_FALSE(culled->hasNextObject());
}

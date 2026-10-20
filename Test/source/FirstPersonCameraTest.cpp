#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>

#include "graphics/camera/FirstPersonCamera.h"

namespace
{
FirstPersonCamera makeCamera()
{
    return FirstPersonCamera(glm::vec3(0.f), glm::vec3(0.f, 0.f, -1.f), 1.f, 1.33f, 0.01f, 1000.f);
}

// The view matrix rows encode the camera basis; forward is the negated third row.
float pitchDegrees(const FirstPersonCamera& camera)
{
    const glm::mat4& view = camera.getView();
    const glm::vec3 forward = -glm::vec3(view[0][2], view[1][2], view[2][2]);
    return glm::degrees(std::asin(forward.y));
}

// The up axis of the camera is the second column of the view matrix.
glm::vec3 upAxis(const FirstPersonCamera& camera)
{
    const glm::mat4& view = camera.getView();
    return glm::vec3(view[0][1], view[1][1], view[2][1]);
}
}  // namespace

TEST_CASE("FirstPersonCamera lookAt keeps a horizontal target level", "[camera]")
{
    auto camera = makeCamera();

    camera.lookAt(glm::vec3(0.f, 0.f, -1.f));

    REQUIRE(pitchDegrees(camera) == Catch::Approx(0.f).margin(0.01f));
}

TEST_CASE("FirstPersonCamera keeps the world up axis", "[camera]")
{
    auto camera = makeCamera();

    camera.lookAt(glm::vec3(0.f, 0.f, -1.f));

    // The camera has no roll, so its up axis stays the world up axis.
    REQUIRE(upAxis(camera) == glm::vec3(0.f, 1.f, 0.f));
}

TEST_CASE("FirstPersonCamera lookAt clamps the elevation of the target", "[camera]")
{
    auto camera = makeCamera();

    // Within the range the camera allows the elevation is kept.
    camera.lookAt(glm::vec3(0.f), glm::vec3(0.f, 0.5f, -1.f));
    REQUIRE(pitchDegrees(camera) == Catch::Approx(26.565f).margin(0.1f));

    // Beyond it the camera is limited to 45 degrees, the same limit pitch has.
    camera.lookAt(glm::vec3(0.f), glm::vec3(0.f, 2.f, -1.f));
    REQUIRE(pitchDegrees(camera) == Catch::Approx(45.f).margin(0.1f));
}

TEST_CASE("FirstPersonCamera lookAt survives a target straight above", "[camera]")
{
    auto camera = makeCamera();

    camera.lookAt(glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));

    REQUIRE(std::isfinite(pitchDegrees(camera)));
}

TEST_CASE("FirstPersonCamera lookAt survives a target at the eye position", "[camera]")
{
    auto camera = makeCamera();

    camera.lookAt(glm::vec3(2.f, 3.f, 4.f), glm::vec3(2.f, 3.f, 4.f));

    REQUIRE(std::isfinite(pitchDegrees(camera)));
}
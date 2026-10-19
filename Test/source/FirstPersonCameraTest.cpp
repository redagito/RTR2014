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
    return FirstPersonCamera(glm::vec3(0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f),
                              1.f, 1.33f, 0.01f, 1000.f);
}

// The view matrix rows encode the camera basis; forward is the negated third row.
float pitchDegrees(const FirstPersonCamera& camera)
{
    const glm::mat4& view = camera.getView();
    const glm::vec3 forward = -glm::vec3(view[0][2], view[1][2], view[2][2]);
    return glm::degrees(std::asin(forward.y));
}
} // namespace

TEST_CASE("FirstPersonCamera lookAt keeps a horizontal target level", "[camera]")
{
    auto camera = makeCamera();

    camera.lookAt(glm::vec3(0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f));

    REQUIRE(pitchDegrees(camera) == Catch::Approx(0.f).margin(0.01f));
}

TEST_CASE("FirstPersonCamera lookAt preserves the elevation of the target", "[camera]")
{
    auto camera = makeCamera();

    camera.lookAt(glm::vec3(0.f), glm::vec3(0.f, 1.f, -1.f), glm::vec3(0.f, 1.f, 0.f));
    REQUIRE(pitchDegrees(camera) == Catch::Approx(45.f).margin(0.1f));

    camera.lookAt(glm::vec3(0.f), glm::vec3(0.f, 0.5f, -1.f), glm::vec3(0.f, 1.f, 0.f));
    REQUIRE(pitchDegrees(camera) == Catch::Approx(26.565f).margin(0.1f));
}

TEST_CASE("FirstPersonCamera lookAt survives a target straight above", "[camera]")
{
    auto camera = makeCamera();

    camera.lookAt(glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 1.f, 0.f));

    REQUIRE(std::isfinite(pitchDegrees(camera)));
}

TEST_CASE("FirstPersonCamera lookAt survives a target at the eye position", "[camera]")
{
    auto camera = makeCamera();

    camera.lookAt(glm::vec3(2.f, 3.f, 4.f), glm::vec3(2.f, 3.f, 4.f), glm::vec3(0.f, 1.f, 0.f));

    REQUIRE(std::isfinite(pitchDegrees(camera)));
}
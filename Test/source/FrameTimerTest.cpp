#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "util/FrameTimer.h"

TEST_CASE("FrameTimer reports the time between two frames", "[util]")
{
    FrameTimer timer(0.0);

    REQUIRE(timer.tick(0.02) == Catch::Approx(0.02f));
    REQUIRE(timer.tick(0.05) == Catch::Approx(0.03f));
}

TEST_CASE("FrameTimer clamps a long frame", "[util]")
{
    FrameTimer timer(0.0);

    REQUIRE(timer.tick(5.0) == Catch::Approx(0.1f));
}

TEST_CASE("FrameTimer keeps measuring after a clamped frame", "[util]")
{
    FrameTimer timer(0.0);

    REQUIRE(timer.tick(5.0) == Catch::Approx(0.1f));
    REQUIRE(timer.tick(5.02) == Catch::Approx(0.02f));
}

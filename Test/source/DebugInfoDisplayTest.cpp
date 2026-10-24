#include <catch2/catch_test_macros.hpp>

#include "graphics/DebugInfoDisplay.h"

TEST_CASE("Debug panel layout fills the default window", "[debug]")
{
    const DebugPanelLayout layout = computeDebugPanelLayout(12, 10, 31, 800, 600);

    REQUIRE(layout.keyX == 360);
    REQUIRE(layout.settingX == 430);
    REQUIRE(layout.statusX == 600);
    REQUIRE(layout.logBottom == 120);
    REQUIRE(layout.rowsTop == 600);
    REQUIRE(layout.panelWidth == 800);
    REQUIRE(layout.visibleRows == 31);
}

TEST_CASE("Debug panel layout stretches with a wider window", "[debug]")
{
    const DebugPanelLayout layout = computeDebugPanelLayout(12, 10, 31, 1280, 720);

    REQUIRE(layout.panelWidth == 1280);
    REQUIRE(layout.rowsTop == 720);
    REQUIRE(layout.visibleRows == 31);
}

TEST_CASE("Debug panel layout clips rows that would overlap the log", "[debug]")
{
    // The log takes the bottom 120 px, leaving 280 px for rows of 12 px.
    const DebugPanelLayout layout = computeDebugPanelLayout(12, 10, 31, 800, 400);

    REQUIRE(layout.visibleRows == 23);
    REQUIRE(layout.rowsTop - layout.visibleRows * 12 >= layout.logBottom);
}

TEST_CASE("Debug panel layout hides the rows when there is no room", "[debug]")
{
    REQUIRE(computeDebugPanelLayout(12, 10, 31, 800, 120).visibleRows == 0);
    REQUIRE(computeDebugPanelLayout(12, 10, 31, 800, 60).visibleRows == 0);
    REQUIRE(computeDebugPanelLayout(12, 10, 0, 800, 600).visibleRows == 0);
}

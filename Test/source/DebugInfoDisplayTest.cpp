#include <catch2/catch_test_macros.hpp>

#include "graphics/DebugInfoDisplay.h"

TEST_CASE("Debug panel layout fits the content in the default window", "[debug]")
{
    const DebugPanelLayout layout = computeDebugPanelLayout(12, 10, 31, 800, 600);

    REQUIRE(layout.keyX == 6);
    REQUIRE(layout.settingX == 62);
    REQUIRE(layout.statusX == 195);
    REQUIRE(layout.logBottom == 120);
    REQUIRE(layout.rowsTop == 600);
    REQUIRE(layout.panelWidth == 348);
    REQUIRE(layout.visibleRows == 31);
}

TEST_CASE("Debug panel layout keeps its width when the window grows", "[debug]")
{
    const DebugPanelLayout layout = computeDebugPanelLayout(12, 10, 31, 1280, 720);

    REQUIRE(layout.panelWidth == 348);
    REQUIRE(layout.rowsTop == 720);
    REQUIRE(layout.visibleRows == 31);
}

TEST_CASE("Debug panel layout scales its columns with the font", "[debug]")
{
    const DebugPanelLayout layout = computeDebugPanelLayout(22, 10, 31, 1920, 1080);

    REQUIRE(layout.keyX == 11);
    REQUIRE(layout.settingX == 115);
    REQUIRE(layout.statusX == 362);
    REQUIRE(layout.logBottom == 220);
    REQUIRE(layout.panelWidth == 646);
    REQUIRE(layout.visibleRows == 31);
}

TEST_CASE("Debug panel layout is clipped by a narrow window", "[debug]")
{
    const DebugPanelLayout layout = computeDebugPanelLayout(12, 10, 31, 200, 600);

    REQUIRE(layout.panelWidth == 200);
    REQUIRE(layout.keyX < layout.panelWidth);
}

TEST_CASE("Debug font size scales with the window height", "[debug]")
{
    REQUIRE(computeDebugFontSize(600) == 12);
    REQUIRE(computeDebugFontSize(720) == 14);
    REQUIRE(computeDebugFontSize(1080) == 22);
    REQUIRE(computeDebugFontSize(1440) == 29);
    REQUIRE(computeDebugFontSize(2160) == 32);
}

TEST_CASE("Debug font size stays within the font atlas", "[debug]")
{
    REQUIRE(computeDebugFontSize(0) == 12);
    REQUIRE(computeDebugFontSize(100) == 12);
    REQUIRE(computeDebugFontSize(10000) == 32);
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

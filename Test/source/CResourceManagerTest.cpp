#include <catch2/catch_test_macros.hpp>

#include "resource/ResourceConfig.h"
#include "resource/core/CResourceManager.h"

TEST_CASE("loadImage rejects a file that is not a png", "[resource]")
{
    CResourceManager manager;

    REQUIRE(manager.loadImage("not_an_image.txt", EColorFormat::RGBA32) == invalidResource);
}

TEST_CASE("loadImage rejects an unknown color format", "[resource]")
{
    CResourceManager manager;

    REQUIRE(manager.loadImage("image.png", EColorFormat::Invalid) == invalidResource);
}
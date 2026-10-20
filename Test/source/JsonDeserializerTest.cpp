#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <limits>

#include <json/json.h>

#include "io/JsonDeserializer.h"

TEST_CASE("JsonDeserializer rejects a number that does not fit an int64", "[io]")
{
    Json::Value node(static_cast<uint64_t>(std::numeric_limits<uint64_t>::max()));

    int64_t value = 0;
    REQUIRE_FALSE(deserialize(node, value));
}

TEST_CASE("JsonDeserializer rejects a number that does not fit an int32", "[io]")
{
    Json::Value node(static_cast<uint64_t>(std::numeric_limits<uint64_t>::max()));

    int32_t value = 0;
    REQUIRE_FALSE(deserialize(node, value));
}

TEST_CASE("JsonDeserializer rejects a negative number for a uint32", "[io]")
{
    Json::Value node(static_cast<int64_t>(-1));

    uint32_t value = 0;
    REQUIRE_FALSE(deserialize(node, value));
}

TEST_CASE("JsonDeserializer rejects a real for an integer", "[io]")
{
    Json::Value node(1.5);

    int64_t value = 0;
    REQUIRE_FALSE(deserialize(node, value));
}

TEST_CASE("JsonDeserializer accepts a real for a double", "[io]")
{
    Json::Value node(3.0);

    double value = 0.0;
    REQUIRE(deserialize(node, value));
    REQUIRE(value == 3.0);
}

TEST_CASE("JsonDeserializer accepts the largest int64", "[io]")
{
    Json::Value node(static_cast<int64_t>(std::numeric_limits<int64_t>::max()));

    int64_t value = 0;
    REQUIRE(deserialize(node, value));
    REQUIRE(value == std::numeric_limits<int64_t>::max());
}

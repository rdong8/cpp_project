#include <catch2/catch_test_macros.hpp>

import mathematics;

// TEST_CASE("test name", "[tags]...")
TEST_CASE("Vector algebra", "[Vec2][math]")
{
    // Setup (reran for each section)
    constexpr math::Vec2 v{2, 3};
    constexpr math::Vec2 w{4, 5};

    // SECTION("section name", "[section description]...")
    SECTION("Dot product is commutative")
    {
        REQUIRE(v.dot(w) == w.dot(v));
    }

    SECTION("Scalar dot product is associative")
    {
        constexpr double c{3.0};
        REQUIRE(c * (v.dot(w)) == (c * v).dot(w));
    }
}

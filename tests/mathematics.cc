#include <catch2/catch_test_macros.hpp>

import mathematics;

// TEST_CASE("test name", "[tags]...")
TEST_CASE("Vector algebra", "[Vec2][math]")
{
    // Setup (reran for each section)
    math::Vec2 constexpr v{2, 3};
    math::Vec2 constexpr w{4, 5};

    // SECTION("section name", "[section description]...")
    SECTION("Dot product is commutative")
    {
        REQUIRE(v.dot(w) == w.dot(v));
    }

    SECTION("Scalar dot product is associative")
    {
        double constexpr c{3.0};
        REQUIRE(c * (v.dot(w)) == (c * v).dot(w));
    }
}

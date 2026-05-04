module;

#include <catch2/catch_test_macros.hpp>

#include "test_main.hpp"

module mathematics;

// TODO: move into namespace math
TEST_CASE("Vector algebra", "[Vec][math]")
{
    // Setup (reran for each section)
    math::Vec<2> constexpr v{2., 3.};
    math::Vec<2> constexpr w{4., 5.};

    // SECTION("section name", "[section description]...")
    SECTION("Dot product is commutative")
    {
        REQUIRE(v.dot(w) == w.dot(v));
    }

    SECTION("Scalar dot product is associative")
    {
        auto constexpr c{3.0};
        REQUIRE(c * (v.dot(w)) == (c * v).dot(w));
    }
}

TEST_MAIN()

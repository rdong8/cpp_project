module;

#include <catch2/catch_test_macros.hpp>

export module library_usage.test_mathematics;

import library_usage.mathematics;

TEST_CASE("Vector algebra", "[Vec][math]")
{
    // Setup (reran for each section)
    math::Vec<2> constexpr V{2., 3.};
    math::Vec<2> const w{4., 5.};

    // SECTION("section name", "[section description]...")
    SECTION("Dot product is commutative")
    {
        REQUIRE(V.dot(w) == w.dot(V));
    }

    SECTION("Scalar dot product is associative")
    {
        auto constexpr C{3.0};
        REQUIRE(C * (V.dot(w)) == (C * V).dot(w));
    }
}

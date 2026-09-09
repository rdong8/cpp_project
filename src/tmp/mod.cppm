/// @file
/// @brief Temporary target to test changes during Bazel transition

export module rdong8.tmp;

import std;

/// A function
export auto hello_world() -> void
{
    std::println("Hello World");
}

module;

#include <spdlog/spdlog.h>

export module my_module;

import std;

// Use some C++20/23 features

export class Example
{
public:
    auto say_hello(this auto const &self) -> void
    {
        std::println("Hello from println! x = {}", self.x);
        spdlog::info("Hello from spdlog! x = {}", self.x);
    }

    auto operator<=>(const Example&) const = default;

private:
    int x{};
};


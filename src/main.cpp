#include <spdlog/spdlog.h>

// Use some C++20/23 features

class Example
{
public:
    auto say_hello(this auto const &self) -> void
    {
        spdlog::info("Hello! x = {}", self.x);
    }

    auto operator<=>(const Example&) const = default;

private:
    int x{};
};

auto main() -> int
{
    Example example{};
    example.say_hello();
}

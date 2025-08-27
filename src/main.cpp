import std;

namespace
{

// Use some C++20/23 features

class Example
{
public:
    auto say_hello(this auto const &self) -> void
    {
        std::println("Hello from Example! x = {}", self.x);
    }

    auto operator<=>(const Example&) const = default;

private:
    int x{};
};

}

auto main() -> int
{
    Example example{};
    example.say_hello();
}

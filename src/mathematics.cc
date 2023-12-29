#include <spdlog/fmt/fmt.h>

#include "mathematics.h"

[[nodiscard]]
auto fmt::formatter<math::Vec2>::format(const math::Vec2& vec, fmt::format_context& ctx) -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "<{}, {}>", vec.x(), vec.y());
}

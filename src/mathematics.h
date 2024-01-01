/// @file
/// A sample library containing math functions.
/// A documentation comment for the file like this one must be in any file you wish to be tracked by Doxygen.
/// Note that `constexpr` functions are implicitly inlined and thus will be implemented in this header file. Non-inline
/// functions will be implemented in the `.cc` file.

#pragma once

#include <cmath>
#include <compare>
#include <concepts>

#include <spdlog/fmt/fmt.h>

namespace math {
    /// A 2-dimensional mathematical vector
    class Vec2 {
        /// The x component of the vector (regular doc comment)
        double x_;
        double y_; ///< The y component of the vector (postfix doc comment)

    public:
        /// Construct a new @ref Vec2 with the given components.
        /// @param[in] _x The x component
        /// @param[in] _y The y component
        constexpr Vec2(double _x, double _y)
            : x_{_x}, y_{_y} {
        }

        /// @name Rule of 5 special member functions
        ///@{
        /// This is a member group. Documentation comments here will be shared for all elements in the group.

        constexpr ~Vec2() = default;

        constexpr Vec2(const Vec2& other) noexcept = default;

        constexpr Vec2(Vec2&& other) noexcept = default;

        [[maybe_unused]]
        constexpr auto operator=(const Vec2& other) noexcept -> Vec2& = default;

        [[maybe_unused]]
        constexpr auto operator=(Vec2&& other) noexcept -> Vec2& = default;

        ///@}

        /// @name Getters
        ///@{
        /// Getters for each component of the vector
        [[nodiscard]]
        constexpr auto x() const noexcept -> double {
            return this->x_;
        }

        [[nodiscard]]
        constexpr auto y() const noexcept -> double {
            return this->y_;
        }

        ///@}

        /// Compute the dot product of this vector and @p other
        /// @param[in] other The second operand of the dot product operator
        [[nodiscard]]
        constexpr auto dot(const Vec2& other) const noexcept -> double {
            return this->x_ * other.x_ + this->y_ * other.y_;
        }

        /// Compute the norm of this vector
        [[nodiscard]]
        constexpr auto norm() const noexcept -> double {
            return std::sqrt(this->dot(*this));
        }

        /// Compare this vector and @p other by norm
        [[nodiscard]]
        constexpr auto operator<=>(const Vec2& other) const noexcept -> std::weak_ordering {
            return std::weak_order(this->norm(), other.norm());
        }

        /// Compute the scalar product of this vector and @p c
        /// @param[in] c The scalar
        [[nodiscard]]
        constexpr auto operator*(double c) const noexcept -> Vec2 {
            return Vec2{c * this->x(), c * this->y()};
        }
    };

    constexpr auto operator*(double c, const Vec2& vec) noexcept -> Vec2 {
        return vec * c;
    }

    /// Evaluate the approximate derivative of @p f at @p x
    /// @tparam f A function @f$ f : \mathbb R \to \mathbb R @f$
    /// @param[in] x The x-value to evaluate the derivative at
    ///
    /// Example: @f$ f(x) = x^2 @f$
    /// @code{.cpp}
    /// auto f{[](double x) { return x * x; }};
    /// std::cout << d_dx<f>(3.0) << '\n'; // Prints 6
    /// @endcode
    template<auto f>
        requires requires(double x)
        {
            { f(x) } -> std::same_as<double>;
        }
    [[nodiscard]]
    constexpr auto d_dx(double x) -> double {
        constexpr static double dx{0.00001};
        return (f(x + dx) - f(x)) / dx;
    }
}

/// Formatter specialization for @ref math::Vec2
template<>
struct fmt::formatter<math::Vec2> {
    [[nodiscard]]
    constexpr auto parse(format_parse_context& ctx) -> format_parse_context::iterator {
        return ctx.end();
    }

    /// This function will be implemented in the implementation file.
    [[nodiscard]]
    auto format(const math::Vec2& vec, fmt::format_context& ctx) -> decltype(ctx.out());
};

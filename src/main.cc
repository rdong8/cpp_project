#include <boost/asio.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "mathematics.h"

namespace {
    /// Configure the default logger to log to the file "log.txt", flushing on the info level or higher
    auto configure_logger() -> void {
        // Create new logger called "logger" to log.txt, clearing previous contents
        const auto logger{spdlog::basic_logger_st("logger", "log.txt", true)};

        // Display the message as "[abbreviated log level] message"
        logger->set_pattern("[%L] %v");

        // Automatically flush for info level or higher
        logger->flush_on(spdlog::level::info);

        // Set default logger to "logger"
        spdlog::set_default_logger(logger);
    }

    // Demonstrate Boost ASIO
    auto asio_demo() -> void {
        spdlog::info("ASIO DEMO:");

        spdlog::info("Starting wait for 2 seconds!");

        boost::asio::io_context io{};
        boost::asio::steady_timer t{io, boost::asio::chrono::seconds{2}};

        t.wait();

        spdlog::info("Finished waiting!");
    }

    // Demonstrate 2D vector class
    auto vec2_demo() -> void {
        spdlog::info("VECTOR DEMO:");

        constexpr math::Vec2 north{0, 1};
        constexpr math::Vec2 east{1, 0};
        constexpr math::Vec2 northeast{1, 1};

        spdlog::info("Dot product of {} and {} is: {}", north, east, north.dot(east));
        spdlog::info("Dot product of {} and {} is: {}", north, northeast, north.dot(northeast));

        spdlog::info("Norm of {} is: {}", northeast, northeast.norm());

        spdlog::info("Is {} < {}? {}", east, northeast, east < northeast);
    }

    // Demonstrate differentiation operator
    auto d_demo() -> void {
        using math::d;

        spdlog::info("DERIVATIVES DEMO:");

        static constexpr auto f{[](double x) { return 3 * x * x - x + 16; }};

        spdlog::info("f(x) = 3x^2 - x + 16");
        spdlog::info("f'(4) = {}", d<f>(4.0));

        constexpr auto dfdx{[](double x) { return d<f>(x); }};

        spdlog::info("f''(4) = {}", d<dfdx>(4.0));
    }
}

auto main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) -> int {
    ::configure_logger();

    ::asio_demo();

    ::vec2_demo();

    ::d_demo();

    return 0;
}

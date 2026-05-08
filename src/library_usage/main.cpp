#include <boost/cobalt/channel.hpp>
#include <boost/cobalt/main.hpp>
#include <boost/cobalt/promise.hpp>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

import std;

import mathematics;

namespace
{

// Configure the default logger to log to the file "log.txt", flushing on the info level or higher
[[maybe_unused]]
auto configure_file_logger() -> void
{
    // Create new logger called "logger" to log.txt, clearing previous contents
    auto const logger{spdlog::basic_logger_st("logger", "log.txt", true)};

    // Change the message format to "[abbreviated log level] message"
    // https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
    logger->set_pattern("[%L] %v");

    // Automatically flush for info level or higher
    logger->flush_on(spdlog::level::info);

    // Set the logger used by the `spdlog::<level>(msg)` logging functions to this logger ie.
    // `spdlog::warn("Warning!!")`
    spdlog::set_default_logger(logger);
}

auto producer(boost::cobalt::channel<int> &channel) -> boost::cobalt::promise<void>
{
    for (auto const i : std::views::iota(0, 5))
    {
        spdlog::info("Producing {}", i);
        co_await channel.write(i);
    }

    channel.close();
}

auto cobalt_demo() -> boost::cobalt::promise<void>
{
    spdlog::info("COBALT DEMO:");

    boost::cobalt::channel<int> channel{};

    auto promise{producer(channel)};

    while (channel.is_open())
    {
        spdlog::info("Consumer received {}", co_await channel.read());
    }

    // Force the producer to finish
    co_await promise;
}

auto vec_demo() -> void
{
    spdlog::info("VECTOR DEMO:");

    math::Vec<2> const north{0., 1.};
    math::Vec<2> const east{1., 0.};
    math::Vec<2> const northeast{1., 1.};

    spdlog::info("Dot product of {} and {} is: {}", north, east, north.dot(east));
    spdlog::info("Dot product of {} and {} is: {}", north, northeast, north.dot(northeast));

    spdlog::info("Norm of {} is: {}", northeast, northeast.norm());

    spdlog::info("Is {} < {}? {}", east, northeast, east < northeast);
}

auto differentiation_demo() -> void
{
    using math::d_dx;

    spdlog::info("DIFFERENTIATION DEMO:");

    // NOLINTBEGIN(readability-magic-numbers)
    auto constexpr F{[](double x) { return 3 * x * x - x + 16; }};
    auto constexpr DF_DX{d_dx<F>};

    spdlog::info("f(x) = 3x^2 - x + 16");
    spdlog::info("f'(4) = {}", DF_DX(4.0));
    spdlog::info("f''(4) = {}", d_dx<DF_DX>(4.0));
    // NOLINTEND(readability-magic-numbers)
}

} // namespace

auto co_main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) -> boost::cobalt::main
{
    // By default spdlog will log to stdout
    // configure_file_logger();

    co_await cobalt_demo();

    vec_demo();

    differentiation_demo();

    co_return 0;
}

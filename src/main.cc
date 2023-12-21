#include <format>
#include <iostream>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

auto main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) -> int {
    // Set up the logger
    const auto logger{spdlog::basic_logger_st("logger", "log.txt", true)};
    logger->set_pattern("[%L] %v");
    logger->flush_on(spdlog::level::info);
    spdlog::set_default_logger(logger);

    spdlog::info("Logging!");
    std::cout << std::format("Hello {}!\n", "Richard");

    return 0;
}

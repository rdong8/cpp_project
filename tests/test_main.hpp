#include <catch2/catch_session.hpp>

// Must be placed module being tested
#define TEST_MAIN()                                                                                                    \
    extern "C++" auto main(int argc, char *argv[]) -> int                                                              \
    {                                                                                                                  \
        return Catch::Session{}.run(argc, argv);                                                                       \
    }

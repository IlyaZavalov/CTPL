cmake_minimum_required(VERSION 3.27)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB_RECURSE CTPL_TESTS_SRCS *.cpp)
add_executable(ctpl_test ${CTPL_TESTS_SRCS})
target_link_libraries(ctpl_test GTest::gtest_main ctpl)

include(GoogleTest)
gtest_discover_tests(ctpl_test)

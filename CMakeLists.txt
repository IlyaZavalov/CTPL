cmake_minimum_required(VERSION 3.27)
project(ctpl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(CTPL_ENABLE_TESTS "if on, tests are enabled" ON)
option(CTPL_ENABLE_PYBIND11 "if on, python can be used with CTPL" ON)

add_subdirectory(ctpl)

if (CTPL_ENABLE_PYBIND11)
    add_subdirectory(3rdparty/pybind11)
    add_subdirectory(ctpl_py)
endif()

if (CTPL_ENABLE_TESTS)
    add_subdirectory(tests)
endif()

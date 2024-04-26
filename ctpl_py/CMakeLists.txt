cmake_minimum_required(VERSION 3.27)

file(GLOB_RECURSE CTPL_PY_SRCS src/*.cpp)
pybind11_add_module(ctpl_py SHARED ${CTPL_PY_SRCS})
target_link_libraries(ctpl_py PRIVATE ctpl)
target_link_libraries(ctpl_py PRIVATE pybind11_headers)

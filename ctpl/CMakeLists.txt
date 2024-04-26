cmake_minimum_required(VERSION 3.27)

file(GLOB_RECURSE CTPL_SRCS src/*.cpp)
add_library(ctpl STATIC ${CTPL_SRCS})
target_include_directories(ctpl PUBLIC include)
target_include_directories(ctpl PRIVATE src)

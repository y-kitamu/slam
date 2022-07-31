cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_COMPILER /usr/bin/clang)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)
# set(CMAKE_CUDA_COMPILER /usr/local/cuda/bin/nvcc)

project(FOO VERSION 0.1 LANGUAGES CXX) # CUDA)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_XCC_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# external deps
add_subdirectory(ext)

# create library
set(LIBRARIES "" CACHE INTERNAL "")
add_subdirectory(src)

# create executables
add_subdirectory(snippet)

# test
if (CMAKE_BUILD_TYPE STREQUAL "Release")
  include(test)
endif()

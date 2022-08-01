cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_COMPILER /usr/bin/clang)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)
# set(CMAKE_CUDA_COMPILER /usr/local/cuda/bin/nvcc)

project(Slam VERSION 0.1 LANGUAGES CXX) # CUDA)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_XCC_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_SNIPPETS "Build snippets" ON)

include(cmake/utility.cmake)

# build external deps
message(STATUS "Add external dependencies...")
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext)
set(EXTERNAL_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/ext)

execute_process_with_output(
  COMMAND rm -rf CMake*
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/ext
)

if (${BUILD_SNIPPETS})
  execute_process_with_output(
    COMMAND cmake -GNinja ${EXTERNAL_DIR}
    WORKING_DIRECTORY ${EXTERNAL_BUILD_DIR}
  )
  execute_process_with_output(
    COMMAND ninja
    WORKING_DIRECTORY ${EXTERNAL_BUILD_DIR}
  )
endif()

# add external deps to the project
set(EXTERNAL_LIBS "" CACHE INTERNAL "")
# eigen
find_package(Eigen3 REQUIRED)
include_directories(AFTER ${EIGEN3_INCLUDE_DIR})

if (${BUILD_SNIPPETS})
  # opengl
  find_package(OpenGL REQUIRED)
  find_package(GLEW REQUIRED)
  # Pangolin
  find_package(Pangolin REQUIRED)

  set(EXTERNAL_LIBS
    ${EXTERNAL_LIBS}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARY}
    ${Pangolin_LIBRARY}
    CACHE INTERNAL "")
  #
  include_directories(AFTER
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIR}
    ${EXTERNAL_BUILD_DIR}/install/Pangolin/include)
endif()

message(STATUS "External libs : ${EXTERNAL_LIBS}")

 # create library
message(STATUS "Create library...")
set(LIBRARIES "" CACHE INTERNAL "")
add_subdirectory(src)

# create executables
message(STATUS "Create executables...")
if (${BUILD_SNIPPETS})
  add_subdirectory(snippets)
endif()

# test
if (CMAKE_BUILD_TYPE STREQUAL "Release")
  message(STATUS "Create test executables...")
  include(test)
endif()

cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_COMPILER /usr/bin/clang)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)
# set(CMAKE_CUDA_COMPILER /usr/local/cuda/bin/nvcc)

project(Slam VERSION 0.1 LANGUAGES CXX) # CUDA)

option(BUILD_SNIPPETS "Build snippets" ON)
option(REBUILD_EXTERNAL "Rebuild external dependencies" OFF)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_XCC_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if (BUILD_SNIPPETS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lprofiler -ltcmalloc")
endif()

include(cmake/utility.cmake)

### build external deps
message(STATUS "Add external dependencies...")
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext)
set(EXTERNAL_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/ext)
# reset cache
execute_process_with_output(
  COMMAND rm -rf CMake*
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/ext
)
# fmt
add_subdirectory(ext/fmt)
# imgui
file(GLOB IMGUI_FILES "${EXTERNAL_DIR}/imgui/imgui*.cpp")
add_library(imgui SHARED
  ${IMGUI_FILES}
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/imgui/backends/imgui_impl_opengl3.cpp)

if (${BUILD_SNIPPETS})
  # TODO : gitがおかしくなるのでexternalpoject_addを使うのをやめる
  if (${REBUILD_EXTERNAL})
    execute_process_with_output(
      COMMAND cmake -GNinja ${EXTERNAL_DIR}
      WORKING_DIRECTORY ${EXTERNAL_BUILD_DIR}
    )
    execute_process_with_output(
      COMMAND ninja -j 8
      WORKING_DIRECTORY ${EXTERNAL_BUILD_DIR}
    )
  endif()
endif()

### add external deps to the project
set(EXTERNAL_LIBS "" CACHE INTERNAL "")
# eigen
find_package(Eigen3 REQUIRED)
# opencv
set(OpenCV_ROOT ${EXTERNAL_BUILD_DIR}/install)
find_package(OpenCV REQUIRED)
# g2o
set(g2o_ROOT ${EXTERNAL_DIR}/g2o/build/install)
find_package(g2o REQUIRED)
#
include_directories(AFTER
  ${EIGEN3_INCLUDE_DIR}
  ${OpenCV_INCLUDE_DIRS}
  ${EXTERNAL_DIR}/argparse/include
  ${EXTERNAL_DIR}/fmt/include
  ${EXTERNAL_DIR}/imgui
  ${EXTERNAL_DIR}/imgui/backends
  ${g2o_ROOT}/include
)
#
set(EXTERNAL_LIBS
  ${EXTERNAL_LIBS}
  fmt::fmt
  imgui
  ${OpenCV_LIBS}
  g2o::core
  CACHE INTERNAL "")

if (${BUILD_SNIPPETS})
  # opengl
  find_package(OpenGL REQUIRED)
  find_package(GLEW REQUIRED)
  # Pangolin
  set(Pangolin_ROOT ${EXTERNAL_BUILD_DIR}/install)
  find_package(Pangolin REQUIRED)
  # glog
  find_package(glog REQUIRED)

  set(EXTERNAL_LIBS
    ${EXTERNAL_LIBS}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARY}
    ${Pangolin_LIBRARY}
    glog::glog
    CACHE INTERNAL "")
  #
  include_directories(AFTER
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIR}
    ${EXTERNAL_BUILD_DIR}/install/Pangolin/include
  )
endif()

message(STATUS "External libs : ${EXTERNAL_LIBS}")

### create library
message(STATUS "Create library...")
set(LIBRARIES "" CACHE INTERNAL "")
include_directories(AFTER ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(src)

### create executables
message(STATUS "Create executables...")
if (${BUILD_SNIPPETS})
  add_subdirectory(snippets)
endif()

### test
if (CMAKE_BUILD_TYPE STREQUAL "Release")
  message(STATUS "Create test executables...")
  include(test)
endif()

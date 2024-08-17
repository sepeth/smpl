cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(shad)

find_package(PkgConfig REQUIRED)
pkg_check_modules(EPOXY REQUIRED epoxy)
include_directories(${EPOXY_INCLUDE_DIRS})

add_library(${PROJECT_NAME} STATIC shader.cc)
target_include_directories(${PROJECT_NAME} PUBLIC include)

set(EXPORT_DIR "${CMAKE_CURRENT_BINARY_DIR}/cmake")
export(
  TARGETS ${PROJECT_NAME}
  FILE "${EXPORT_DIR}/${PROJECT_NAME}Config.cmake"
  NAMESPACE Shader::
)

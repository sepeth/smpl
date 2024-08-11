cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(main)

find_package(PkgConfig REQUIRED)
pkg_check_modules(EPOXY REQUIRED epoxy)
pkg_check_modules(GLFW REQUIRED glfw3)
message("EPOXY_INCLUDE_DIRS: ${EPOXY_INCLUDE_DIRS}")
message("GLFW_INCLUDE_DIRS: ${GLFW_INCLUDE_DIRS}")

include_directories(${EPOXY_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS})
link_libraries(${EPOXY_LINK_LIBRARIES} ${GLFW_LINK_LIBRARIES})

add_executable(send-vec4-from-vs-to-fs send-vec4-from-vs-to-fs.cc)

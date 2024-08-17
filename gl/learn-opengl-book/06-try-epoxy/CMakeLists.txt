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

add_library(shad STATIC IMPORTED)
set_target_properties(
  shad PROPERTIES 
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../libshad/build/libshad.a
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/../libshad/include
)
add_executable(send-vec4-from-vs-to-fs send-vec4-from-vs-to-fs.cc)
add_executable(uniform-in-fs uniform-in-fs.cc)
add_executable(more-attributes more-attributes.cc)
add_executable(more-attributes-with-shad more-attributes-with-shad.cc)
target_link_libraries(more-attributes-with-shad PRIVATE shad)

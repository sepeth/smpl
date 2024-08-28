cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(textures)

find_package(PkgConfig REQUIRED)
pkg_check_modules(EPOXY REQUIRED epoxy)
pkg_check_modules(GLFW REQUIRED glfw3)
message("EPOXY_INCLUDE_DIRS: ${EPOXY_INCLUDE_DIRS}")
message("GLFW_INCLUDE_DIRS: ${GLFW_INCLUDE_DIRS}")

set(STB_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/../stb)

include_directories(${EPOXY_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${STB_INCLUDE_DIRS})
link_libraries(${EPOXY_LINK_LIBRARIES} ${GLFW_LINK_LIBRARIES})

add_library(shad STATIC IMPORTED)
set_target_properties(
  shad PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../libshad/build/libshad.a
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/../libshad/include
)

link_libraries(shad)

add_executable(textures textures.cc)
add_executable(textures_combined textures_combined.cc)
add_executable(textures_combined_orig textures_combined_orig.cc)
add_executable(textures_wrapping textures_wrapping.cc)
add_executable(textures_combined_mix textures_combined_mix.cc)
add_executable(ex7.3_textures ex7.3_textures.cc)
add_executable(textures_exercise3_orig textures_exercise3_orig.cc)

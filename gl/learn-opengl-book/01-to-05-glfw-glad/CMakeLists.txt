cmake_minimum_required(VERSION 3.20)

# # use homebrew installed llvm
# set(llvm_dir /opt/homebrew/opt/llvm)
# set(CMAKE_C_COMPILER ${llvm_dir}/bin/clang)
# set(CMAKE_CXX_COMPILER ${llvm_dir}/bin/clang++)
# link_directories(${llvm_dir}/lib)
# include_directories(${llvm_dir}/include)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(main)
find_package(glfw3 3 REQUIRED)

# glad
set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../glad-opengl3.3-no-ext)
include_directories(${GLAD_DIR}/include)
aux_source_directory(${GLAD_DIR}/src GLAD_SRC)

aux_source_directory(. SRC_LIST)
add_executable(first-glfw-glad main.cc ${GLAD_SRC})
add_executable(draw-rect draw-rect.cc ${GLAD_SRC})
add_executable(two-triangles-without-EBO two-triangles-without-EBO.cc ${GLAD_SRC})
add_executable(two-triangles-with-EBO two-triangles-with-EBO.cc ${GLAD_SRC})
add_executable(two-triangles-with-different-VAOs-and-VBOs two-triangles-with-different-VAOs-and-VBOs.cc ${GLAD_SRC})
add_executable(two-triangles-with-different-colors two-triangles-with-different-colors.cc ${GLAD_SRC})
add_executable(max-vertex-attrib max-vertex-attrib.cc ${GLAD_SRC})
target_link_libraries(first-glfw-glad PRIVATE glfw)
target_link_libraries(draw-rect PRIVATE glfw)
target_link_libraries(two-triangles-without-EBO PRIVATE glfw)
target_link_libraries(two-triangles-with-EBO PRIVATE glfw)
target_link_libraries(two-triangles-with-different-VAOs-and-VBOs PRIVATE glfw)
target_link_libraries(two-triangles-with-different-colors PRIVATE glfw)
target_link_libraries(max-vertex-attrib PRIVATE glfw)


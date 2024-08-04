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
set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../opengl/glad-opengl3.3-no-ext)
include_directories(${GLAD_DIR}/include)
aux_source_directory(${GLAD_DIR}/src GLAD_SRC)

aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST} ${GLAD_SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)


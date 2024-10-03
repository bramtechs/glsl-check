cmake_minimum_required(VERSION 3.5)
project(glsl-check CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(GLEW REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(cxxopts CONFIG REQUIRED)

add_executable(${PROJECT_NAME} check.cc)
target_link_libraries(${PROJECT_NAME} PRIVATE
  GLEW::GLEW
  spdlog::spdlog
  glfw
  cxxopts::cxxopts
)

cmake_minimum_required(VERSION 3.20)
project(siml LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.h")

add_library(siml_compiler STATIC ${SOURCE_FILES})
add_library(siml::compiler ALIAS siml_compiler)

target_include_directories(siml_compiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

enable_testing()

add_executable(siml_tests tests/main.cpp)

target_link_libraries(siml_tests PRIVATE siml::compiler)

target_include_directories(siml_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_test(NAME siml_tests COMMAND siml_tests)
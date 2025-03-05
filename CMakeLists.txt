cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

find_package(Catch2 3 REQUIRED)

project(torq
  VERSION 0.0.1
  LANGUAGES CXX)

add_executable(torq
    src/main.cpp)

target_include_directories(torq PRIVATE clipp/include)


add_executable(tests
    tests/lexer.cpp src/parser/lexer.cpp)

target_include_directories(tests PRIVATE Catch2/src/catch2)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

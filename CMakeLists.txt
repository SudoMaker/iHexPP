cmake_minimum_required(VERSION 3.14)
project(iHexPP)

set(CMAKE_CXX_STANDARD 17)

add_library(iHexPP iHexPP.hpp iHexPP.cpp)
target_include_directories(iHexPP PUBLIC .)

add_executable(iHexPP_Test main.cpp)
target_link_libraries(iHexPP_Test iHexPP)
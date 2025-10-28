cmake_minimum_required(VERSION 3.31)
project(TwoUsers)

set(CMAKE_CXX_STANDARD 23)

find_package(unofficial-uwebsockets CONFIG REQUIRED)
add_executable(TwoUsers main.cpp)

target_link_libraries(TwoUsers PRIVATE unofficial::uwebsockets::uwebsockets)


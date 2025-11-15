cmake_minimum_required(VERSION 3.31)
project(SnakeServer)

set(CMAKE_CXX_STANDARD 23)

find_package(unofficial-uwebsockets CONFIG REQUIRED)
add_executable(SnakeServer main.cpp
        TwoClientRooms/src/WSManager.cpp
        include/ServerCore.h
        TwoClientRooms/include/ICore.h
        include/SnakeServer.h
        src/SnakeServer.cpp
        include/Snake.h
        include/utils.h
        src/Snake.cpp
        include/gamecodes.h
)

target_include_directories(SnakeServer PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/TwoClientRooms/include
)
target_link_libraries(SnakeServer PRIVATE unofficial::uwebsockets::uwebsockets)

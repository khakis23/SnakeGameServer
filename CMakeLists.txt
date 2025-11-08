cmake_minimum_required(VERSION 3.31)
project(TwoUsers)

set(CMAKE_CXX_STANDARD 23)

find_package(unofficial-uwebsockets CONFIG REQUIRED)
add_executable(TwoUsers main.cpp
        src/WSManager.cpp
        include/SimpleMsgSender.h
        src/ServerCore.cpp
        include/ServerCore.h
        include/ICore.h
        include/SnakeServer.h
        src/SnakeServer.cpp
        include/SnakeGame.h
        include/utils.h
)

target_include_directories(TwoUsers PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(TwoUsers PRIVATE unofficial::uwebsockets::uwebsockets)

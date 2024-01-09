cmake_minimum_required(VERSION 3.18)
project(SimOhen)

set(CMAKE_CXX_STANDARD 11)

set(PROJECT_HEADERS
        Class/Biotop.h
        Class/Simulacia.h
        Class/my_socket.h
)
set(PROJECT_SOURCES
        main.cpp
        Class/my_socket.cpp
)

add_executable(SimOhen ${PROJECT_SOURCES} ${PROJECT_HEADERS})

target_link_libraries(SimOhen pthread wsock32 ws2_32)
add_executable(Server Server/Server.c)

add_custom_target(All)
add_custom_target(
        Clean
        COMMAND rm -f SimOhen
        COMMAND rm -f Server
)

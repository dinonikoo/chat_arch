cmake_minimum_required(VERSION 3.10)

project(chat)

set(SERVER_DIR "${CMAKE_SOURCE_DIR}/server")
set(CLIENT_DIR "${CMAKE_SOURCE_DIR}/client")

set(SERVER_SOURCES
    ${SERVER_DIR}/main.cpp
    ${SERVER_DIR}/server.hpp
    ${SERVER_DIR}/server.cpp
)

set(CLIENT_SOURCES
    ${CLIENT_DIR}/main.cpp
    ${CLIENT_DIR}/client.hpp
)

add_executable(server ${SERVER_SOURCES})
add_executable(client ${CLIENT_SOURCES})
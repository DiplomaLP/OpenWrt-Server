cmake_minimum_required(VERSION 3.5)
project(OpenWrt_Server)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.c server.c server.h server_subscribers.c server_subscribers.h)
add_executable(OpenWrt_Server ${SOURCE_FILES})

set(SOURCE_FILES_TEST server.c test/test.cpp test/client.cpp)
add_executable(OpenWrt_Server_Test ${SOURCE_FILES_TEST})
TARGET_LINK_LIBRARIES(OpenWrt_Server_Test pthread)

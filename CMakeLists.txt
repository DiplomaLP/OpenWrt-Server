cmake_minimum_required(VERSION 3.5)
project(OpenWrt_Server)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES main.c server.c server.h)
add_executable(OpenWrt_Server ${SOURCE_FILES})
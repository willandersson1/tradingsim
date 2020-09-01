cmake_minimum_required(VERSION 3.10.2)

project(tradingsim)

include_directories(core_code)
include_directories(core_code/headers)

file(GLOB SOURCES "core_code/*.cpp")
file(GLOB PRINTER "helper_code/*.cpp")

set(ALL_SOURCES ${PRINTER} ${SOURCES})

add_executable(tradingsim ${ALL_SOURCES})


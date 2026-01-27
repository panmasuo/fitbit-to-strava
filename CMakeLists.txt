cmake_minimum_required(VERSION 3.22.1)
project(fitbit-to-strava VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall)

set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB SOURCES ${SOURCES_DIR}/*.cpp)
include_directories(${INCLUDE_DIR})

find_package(cpr REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(fitbit ${SOURCES})
target_link_libraries(fitbit cpr::cpr nlohmann_json::nlohmann_json)

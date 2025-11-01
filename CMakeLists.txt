cmake_minimum_required(VERSION 3.20)

project(Server   
         CXX )

set(CMAKE_CXX_VERSION 20)
set(CMAKE_EXTENSION off)

file(GLOB SOURCE src/*.cpp)
add_executable(Server ${SOURCE} )

target_include_directories(Server PUBLIC include)
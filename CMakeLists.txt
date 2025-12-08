cmake_minimum_required(VERSION 3.20)
project(Server CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


find_package(PkgConfig REQUIRED)

pkg_check_modules(FFMPEG REQUIRED
    libavformat
    libavcodec
    libavutil
    libswscale
    libswresample
)
add_executable(Server src/main.cpp)
# add_executable(server temp/ffmepg_demo.cpp)
target_link_libraries(Server
    PRIVATE
        net
        rtsp
    PUBLIC
)

target_include_directories(Server PUBLIC include)
target_include_directories(Server PUBLIC
    ${FFMPEG_INCLUDE_DIRS}
)

target_link_libraries(Server PUBLIC
    ${FFMPEG_LIBRARIES}
)

file(GLOB_RECURSE RTSP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/handlers/*.cpp"
)

add_library(rtsp ${RTSP_SOURCES})

target_include_directories(rtsp PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

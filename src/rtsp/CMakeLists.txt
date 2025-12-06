add_library(rtsp
    rtsp_parser.cpp
    rtsp_dispatcher.cpp
    rtsp_handler.cpp
    rtsp_response_builder.cpp
)

target_include_directories(rtsp PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

add_library(net
    epoll.cpp
    connection.cpp
)

target_include_directories(net PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

add_library(common STATIC
    logger.cpp
    config.cpp
    timer.cpp
    thread_pool.cpp
    socket.cpp
    http.cpp
    http_client.cpp
)

target_include_directories(common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

find_package(Threads REQUIRED)
target_link_libraries(common PRIVATE Threads::Threads)

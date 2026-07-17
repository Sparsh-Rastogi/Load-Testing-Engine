add_library(worker_lib STATIC
    worker.cpp
)

target_include_directories(worker_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(worker_lib PUBLIC common)

add_executable(worker
    main.cpp
)

target_link_libraries(worker PRIVATE worker_lib)

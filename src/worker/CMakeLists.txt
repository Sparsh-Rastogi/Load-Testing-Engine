add_executable(worker
    main.cpp
    worker.cpp
)

target_include_directories(worker PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(worker PRIVATE common)

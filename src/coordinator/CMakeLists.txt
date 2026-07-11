add_executable(coordinator
    main.cpp
    coordinator.cpp
)

target_include_directories(coordinator PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(coordinator PRIVATE common)

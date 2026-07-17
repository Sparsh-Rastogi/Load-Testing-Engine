add_library(coordinator_lib STATIC
    coordinator.cpp
)

target_include_directories(coordinator_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(coordinator_lib PUBLIC common)

add_executable(coordinator
    main.cpp
)

target_link_libraries(coordinator PRIVATE coordinator_lib)

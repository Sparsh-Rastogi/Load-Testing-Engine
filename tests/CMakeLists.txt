include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
)
# Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Define test executables
add_executable(run_common_tests
    common/test_common.cpp
)
target_include_directories(run_common_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
target_link_libraries(run_common_tests PRIVATE
    common
    gtest_main
)

add_executable(run_coordinator_tests
    coordinator/test_coordinator.cpp
)
target_include_directories(run_coordinator_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
target_link_libraries(run_coordinator_tests PRIVATE
    common
    gtest_main
)

add_executable(run_worker_tests
    worker/test_worker.cpp
)
target_include_directories(run_worker_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
target_link_libraries(run_worker_tests PRIVATE
    common
    gtest_main
)

# Register tests with CTest
add_test(NAME CommonTests COMMAND run_common_tests)
add_test(NAME CoordinatorTests COMMAND run_coordinator_tests)
add_test(NAME WorkerTests COMMAND run_worker_tests)

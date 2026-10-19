find_package(Catch2 CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

add_executable(RTR2014Test ${SOURCES})

target_link_libraries(RTR2014Test PRIVATE RTR2014 Catch2::Catch2WithMain)

add_test(NAME RTR2014Test COMMAND RTR2014Test)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

add_executable(RTRDemo ${SOURCES})

target_link_libraries(RTRDemo PRIVATE RTR2014)

add_custom_command(TARGET RTRDemo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/data" "$<TARGET_FILE_DIR:RTRDemo>/data"
    VERBATIM)
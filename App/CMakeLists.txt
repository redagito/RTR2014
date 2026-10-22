file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

add_executable(RTRDemo ${SOURCES})

target_link_libraries(RTRDemo PRIVATE RTR2014)

# The data is copied next to the executable, so that it can be run from the
# build directory. It is a target of its own, so that a change of a data file
# is copied right away and not only when the executable is linked again.
file(GLOB_RECURSE DATA_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/data/*")

add_custom_target(copy_data ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/data" "$<TARGET_FILE_DIR:RTRDemo>/data"
    DEPENDS ${DATA_FILES}
    COMMENT "Copying the data to the build directory"
    VERBATIM)

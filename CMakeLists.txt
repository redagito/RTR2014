cmake_minimum_required(VERSION 3.28)

project(RTR2014 LANGUAGES CXX)

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(lodepng CONFIG REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

file(GLOB_RECURSE RTR2014_SOURCES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/*.h)

add_executable(RTRDemo ${RTR2014_SOURCES})

target_compile_features(RTRDemo PRIVATE cxx_std_23)

# glad owns every GL entry point, so GLFW must not pull in the platform GL headers.
target_compile_definitions(RTRDemo PRIVATE
    GLFW_INCLUDE_NONE
    $<$<CONFIG:Debug>:DEBUG>)

target_include_directories(RTRDemo PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(RTRDemo PRIVATE
    glad::glad
    glfw
    glm::glm
    JsonCpp::JsonCpp
    lodepng::lodepng
    tinyobjloader::tinyobjloader)

# Assets are loaded by CWD-relative path at runtime.
add_custom_command(TARGET RTRDemo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/data $<TARGET_FILE_DIR:RTRDemo>/data)
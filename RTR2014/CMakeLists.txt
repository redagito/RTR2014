find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(lodepng CONFIG REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_library(RTR2014 STATIC ${SOURCES} ${HEADERS})

target_compile_features(RTR2014 PUBLIC cxx_std_23)

target_compile_definitions(RTR2014 PUBLIC
    GLFW_INCLUDE_NONE
    $<$<CONFIG:Debug>:DEBUG>)

target_include_directories(RTR2014 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(RTR2014 PUBLIC
    glad::glad
    glfw
    glm::glm
    JsonCpp::JsonCpp
    lodepng::lodepng
    tinyobjloader::tinyobjloader
    spdlog::spdlog)
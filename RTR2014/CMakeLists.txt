find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(stb CONFIG REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_library(RTR2014 STATIC ${SOURCES} ${HEADERS})

target_compile_features(RTR2014 PUBLIC cxx_std_23)

# glad owns every GL entry point, so GLFW must not pull in the platform GL headers.
# App includes glfw3.h directly, so this has to reach consumers.
target_compile_definitions(RTR2014 PUBLIC GLFW_INCLUDE_NONE)

target_include_directories(RTR2014 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(RTR2014
    PUBLIC
        glad::glad
        glfw
        glm::glm
        spdlog::spdlog
    PRIVATE
        JsonCpp::JsonCpp
        stb::stb
        tinyobjloader::tinyobjloader)
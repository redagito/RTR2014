cmake_minimum_required (VERSION 2.8)
project (RTR2014)

# ===
# third party libraries
# ===

if (NOT APPLE)
find_package (PythonInterp 3.4 REQUIRED)
endif()

find_package (OpenGL REQUIRED)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_INSTALL OFF)
option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(JSONCPP_WITH_TESTS OFF)
option(JSONCPP_WITH_POST_BUILD_UNITTEST OFF)
option(JSONCPP_WITH_PKGCONFIG_SUPPORT OFF)

add_subdirectory(libs/glfw)
include_directories(libs/glfw/include)

add_subdirectory(libs/assimp)
include_directories(libs/assimp/include)

add_subdirectory(libs/tinyobjloader)
include_directories(libs/tinyobjloader)

add_subdirectory(libs/jsoncpp)
include_directories(libs/jsoncpp/include)

add_subdirectory(libs/freetype)
include_directories(libs/freetype/include)

include_directories(libs/glm)

file(GLOB LODEPNG_SOURCES ${CMAKE_SOURCE_DIR}/libs/lodepng/*.cpp)
set(THIRD_PARTY_SOURCES ${THIRD_PARTY_SOURCES} ${LODEPNG_SOURCES})
include_directories(libs/lodepng)

# ===
# generate shaders
# ===

#set (SHADER_SRC_DIR "src/shaders/generated")
#
#file (GLOB SHADER_DIRECTORIES "shaders/[^.]*")
#foreach (SHADER_DIRECTORY ${SHADER_DIRECTORIES})
#	get_filename_component(SHADER_NAME ${SHADER_DIRECTORY} NAME)
#	source_group("shaders\\GLSL\\${SHADER_NAME}" shaders/${SHADER_NAME}/*)
#
#	set(SHADER_NAME "${SHADER_NAME}Shader")
#	message("Generating shader '${SHADER_NAME}'.")
#
#	file(READ ${SHADER_DIRECTORY}/vs.glsl SHADER_VS)
#	file(READ ${SHADER_DIRECTORY}/fs.glsl SHADER_FS)
#
#	string(REGEX REPLACE "\n" "\\\\n" SHADER_VS "${SHADER_VS}")
#	string(REGEX REPLACE "\n" "\\\\n" SHADER_FS "${SHADER_FS}")
#
#	configure_file(
#		"${SHADER_SRC_DIR}/Shader.h.in"
#		"${SHADER_SRC_DIR}/${SHADER_NAME}.h"
#		ESCAPE_QUOTES @ONLY )
#	configure_file(
#		"${SHADER_SRC_DIR}/Shader.cpp.in"
#		"${SHADER_SRC_DIR}/${SHADER_NAME}.cpp"
#		ESCAPE_QUOTES @ONLY )
#endforeach()

# ===
# flextGL
# ===

if (NOT APPLE)
add_custom_target(RunFlextGL ALL
    COMMAND ${PYTHON_EXECUTABLE} "${PROJECT_SOURCE_DIR}/libs/flextGL/flextGLgen.py" -Dsrc/generated -Tglfw3 ${PROJECT_SOURCE_DIR}/profile.txt
    DEPENDS ${PROJECT_SOURCE_DIR}/profile.txt
            ${PROJECT_SOURCE_DIR}/libs/flextGL/flextGLgen.py
            ${PROJECT_SOURCE_DIR}/libs/flextGL/flext.py
            ${PROJECT_SOURCE_DIR}/libs/flextGL/templates/glfw3/flextGL.h.template
            ${PROJECT_SOURCE_DIR}/libs/flextGL/templates/glfw3/flextGL.c.template)
endif()

# ===
# demo executable
# ===

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

include_directories(
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_BINARY_DIR}/src
	)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")

file(GLOB_RECURSE SRC_FILES
	${CMAKE_SOURCE_DIR}/src/*.cpp
	${CMAKE_SOURCE_DIR}/src/*.h
	${CMAKE_BINARY_DIR}/src/*.cpp
	${CMAKE_BINARY_DIR}/src/*.h
#	${CMAKE_SOURCE_DIR}/shaders/*.glsl
    ${CMAKE_SOURCE_DIR}/data/shadersource/*.glsl
)

set(SRC_FILES ${SRC_FILES} ${THIRD_PARTY_SOURCES})

if (NOT APPLE)
  set (SRC_FILES "${SRC_FILES};${CMAKE_BINARY_DIR}/src/generated/flextGL.c;${CMAKE_BINARY_DIR}/src/generated/flextGL.h")
endif()

add_executable(RTRDemo ${SRC_FILES})

target_link_libraries(RTRDemo
	${OPENGL_LIBRARY}
	glfw ${GLFW_LIBRARIES}
	assimp
    tinyobjloader
    jsoncpp_lib
    freetype
)

if (NOT APPLE)
add_dependencies(RTRDemo RunFlextGL)
endif()

# ===
# source groups
# ===

function(create_source_groups DIR PARENT_GROUP)
    file (GLOB SUB_DIRS "${DIR}/[^.]*")
    foreach (SUB_DIR ${SUB_DIRS})
        get_filename_component(SUB_DIR_NAME ${SUB_DIR} NAME)
        if(IS_DIRECTORY ${SUB_DIR})
            if (PARENT_GROUP STREQUAL "")
                source_group("${SUB_DIR_NAME}" ${SUB_DIR}/*)
                create_source_groups("${SUB_DIR}" "${SUB_DIR_NAME}")
            else()
                source_group("${PARENT_GROUP}\\${SUB_DIR_NAME}" ${SUB_DIR}/*)
                create_source_groups("${SUB_DIR}" "${PARENT_GROUP}\\${SUB_DIR_NAME}")
            endif()
        endif()
    endforeach()
endfunction()

create_source_groups("${CMAKE_SOURCE_DIR}/src" "src")
create_source_groups("${CMAKE_SOURCE_DIR}/libs" "libs")

#source_group("shaders\\generated" src/shaders/generated/*)
source_group("shaders" ${CMAKE_SOURCE_DIR}/data/shadersource/*)

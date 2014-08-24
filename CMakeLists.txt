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

add_subdirectory(libs/glfw)
add_subdirectory(libs/assimp)

include_directories(libs/glfw/include)
include_directories(libs/assimp/include)
include_directories(libs/glm)

# ===
# generate shaders
# ===

set (SHADER_SRC_DIR "src/shaders/generated")

file (GLOB SHADER_DIRECTORIES "shaders/[^.]*")
foreach (SHADER_DIRECTORY ${SHADER_DIRECTORIES})
	get_filename_component(SHADER_NAME ${SHADER_DIRECTORY} NAME)
	source_group("Shaders\\GLSL\\${SHADER_NAME}" shaders/${SHADER_NAME}/*)
	
	set(SHADER_NAME "${SHADER_NAME}Shader")
	message("Generating shader '${SHADER_NAME}'.")
	
	file(READ ${SHADER_DIRECTORY}/vs.glsl SHADER_VS)
	file(READ ${SHADER_DIRECTORY}/fs.glsl SHADER_FS)
	
	string(REGEX REPLACE "\n" "\\\\n" SHADER_VS "${SHADER_VS}")
	string(REGEX REPLACE "\n" "\\\\n" SHADER_FS "${SHADER_FS}")
	
	configure_file(
		"${SHADER_SRC_DIR}/Shader.h.in" 
		"${SHADER_SRC_DIR}/${SHADER_NAME}.h" 
		ESCAPE_QUOTES @ONLY )
	configure_file(
		"${SHADER_SRC_DIR}/Shader.cpp.in" 
		"${SHADER_SRC_DIR}/${SHADER_NAME}.cpp" 
		ESCAPE_QUOTES @ONLY )
endforeach()

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

include_directories(
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_BINARY_DIR}/src
	)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB_RECURSE SRC_FILES
	${CMAKE_SOURCE_DIR}/src/*.cpp
	${CMAKE_SOURCE_DIR}/src/*.h
	${CMAKE_BINARY_DIR}/src/*.cpp
	${CMAKE_BINARY_DIR}/src/*.h
	${CMAKE_SOURCE_DIR}/shaders/*.glsl
)

if (NOT APPLE)
  set (SRC_FILES "${SRC_FILES};${CMAKE_BINARY_DIR}/src/generated/flextGL.c;${CMAKE_BINARY_DIR}/src/generated/flextGL.h")
endif()

add_executable(RTRDemo ${SRC_FILES})

target_link_libraries(RTRDemo
	${OPENGL_LIBRARY}
	glfw ${GLFW_LIBRARIES}
	assimp
)

if (NOT APPLE)
add_dependencies(RTRDemo RunFlextGL)
endif()

source_group("Debug" src/debug/*)
source_group("Demo" src/demo/*)
source_group("Generated" src/generated/*)
source_group("Renderer" src/renderer/*)
source_group("Renderer\\core" src/renderer/core/*)
source_group("Resource" src/resource/*)
source_group("Sampler" src/sampler/*)
source_group("Scene" src/scene/*)
source_group("Shaders" src/shaders/*)
source_group("Shaders\\Generated" src/shaders/generated/*)

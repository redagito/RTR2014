cmake_minimum_required (VERSION 2.8)
project (RTR2014)

# ===
# third party libraries
# ===

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
	set(SHADER_NAME "${SHADER_NAME}Shader")
	message("Generating shader '${SHADER_NAME}'.")
	
	file(READ ${SHADER_DIRECTORY}/vs.glsl SHADER_VS)
	file(READ ${SHADER_DIRECTORY}/fs.glsl SHADER_FS)
	
	string(REGEX REPLACE "\n" "\\\\n" SHADER_VS "${SHADER_VS}")
	string(REGEX REPLACE "\n" "\\\\n" SHADER_FS "${SHADER_FS}")
	
	configure_file(
		"${SHADER_SRC_DIR}/Shader.h.in" 
		"${SHADER_SRC_DIR}/${SHADER_NAME}.h" 
		ESCAPE_QUOTES
		@ONLY
		)
endforeach()

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

add_executable(RTR2014Demo ${SRC_FILES})

target_link_libraries(RTR2014Demo
	${OPENGL_LIBRARY}
	glfw ${GLFW_LIBRARIES}
	assimp
)

source_group("Demo" src/demo/*)
source_group("Shaders" src/shaders/**/*)
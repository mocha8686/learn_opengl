cmake_minimum_required(VERSION 3.0.0)
project(learn_opengl VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

find_program(iwyu_path NAMES include-what-you-use iwyu REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include ${OPENGL_INCLUDE_DIR})
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_library(glfw SHARED IMPORTED)
set_target_properties(glfw PROPERTIES
	IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/libglfw3.a
	INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/include/GLFW
)

add_library(glad SHARED ${CMAKE_SOURCE_DIR}/lib/glad.c)
set_target_properties(glad PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/include/glad
)

add_library(stb_image SHARED ${CMAKE_SOURCE_DIR}/lib/stb_image.cpp)
set_target_properties(stb_image PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/include/stb_image
)

add_library(glm INTERFACE)
set_target_properties(glm PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/include/glm
)

add_executable(main
	${CMAKE_SOURCE_DIR}/src/main.cpp
	${CMAKE_SOURCE_DIR}/src/camera.cpp
	${CMAKE_SOURCE_DIR}/src/shader.cpp
	${CMAKE_SOURCE_DIR}/src/texture.cpp
)
target_link_libraries(main ${OPENGL_LIBRARIES}
	glad
	glfw
	glm
	stb_image
)
if(APPLE)
	target_link_libraries(main
		"-framework Cocoa"
		"-framework OpenGL"
		"-framework IOKit"
		"-framework CoreVideo"
	)
endif()
set_property(TARGET main PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

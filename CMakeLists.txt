cmake_minimum_required(VERSION 3.0.0)
project(learn_opengl VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)

include(CTest)
enable_testing()

find_package(OpenGL REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include ${OPENGL_INCLUDE_DIR})
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_library(glfw STATIC IMPORTED)
set_target_properties(glfw PROPERTIES
	IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/libglfw3.a
	INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/include/GLFW
)

add_library(glad SHARED ${CMAKE_SOURCE_DIR}/lib/glad.c)
set_target_properties(glad PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/include/glad
)

add_executable(main ${CMAKE_SOURCE_DIR}/src/main.cpp ${CMAKE_SOURCE_DIR}/src/shaders.cpp)
target_link_libraries(main ${OPENGL_LIBRARIES} glfw glad)
if(APPLE)
	target_link_libraries(main
		"-framework Cocoa"
		"-framework OpenGL"
		"-framework IOKit"
		"-framework CoreVideo"
	)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

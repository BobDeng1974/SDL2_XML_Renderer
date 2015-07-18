cmake_minimum_required(VERSION 2.8)

project(SDL2_XML_Renderer)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall")

find_package(SDL2)
find_package(SDL2_image)
find_package(OpenGL)
find_package(GLEW)

if (WIN32)
	add_definitions(-DGLEW_STATIC)
	set(GLM_DIR $ENV{SDK}/glm)
else (WIN32)
	set(GLM_DIR /usr/include/glm)
endif (WIN32)

set(GL_LIBS ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
set(SDL2_LIBS ${SDL2_LIBRARY} ${SDL2IMAGE_LIBRARY})

set(GL_INC ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS})
set(SDL2_INC ${SDL2_INCLUDE_DIR} ${SDL2IMAGE_INCLUDE_DIR})

file(GLOB SOURCES source/*.cpp)
file(GLOB HEADERS include/*.h)
file(GLOB_RECURSE RES Resources/*)

include_directories(${GLM_DIR} ${GL_INC} ${SDL2_INC} include)

add_executable(SDL2_XML_Renderer ${SOURCES} ${HEADERS} ${RES})
target_link_libraries(SDL2_XML_Renderer LINK_PUBLIC ${SDL2_LIBS} ${GL_LIBS})

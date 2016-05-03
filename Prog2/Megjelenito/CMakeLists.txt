cmake_minimum_required(VERSION 2.8)
project(megjelenito)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(Boost 1.38)
find_package(Osmium COMPONENTS io sparsehash)

include(OsmiumOptions)

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})
if(NOT GLUT_FOUND)
    message(ERROR " GLUT not found!")
endif(NOT GLUT_FOUND)

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)


add_executable(${PROJECT_NAME} "src/megjelenito.cpp")
target_link_libraries(${PROJECT_NAME} ${OSMIUM_LIBRARIES} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

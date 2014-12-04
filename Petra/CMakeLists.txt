cmake_minimum_required(VERSION 2.8)
project(petra)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(Boost 1.38)
find_package(Osmium COMPONENTS io sparsehash)

include(OsmiumOptions)

add_executable(${PROJECT_NAME} "src/petra.cpp")
target_link_libraries(${PROJECT_NAME} ${OSMIUM_LIBRARIES})

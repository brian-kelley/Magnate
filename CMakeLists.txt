cmake_minimum_required (VERSION 3.0)
project (Magnate)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_executable (Magnate
include/Atlas
include/Building
include/Button
include/Chunk
include/Constants
include/ScrollBlock
include/Component
include/Control
include/Coord
include/Cuboid
include/SaveManager
include/Field
include/Label
include/Model
include/Scene
include/View
include/World
include/MultiSelect
src/main
src/Atlas
src/Building
src/Button
src/Chunk
src/Constants
src/ScrollBlock
src/Component
src/Control
src/Coord
src/Cuboid
src/SaveManager
src/Field
src/Label
src/Model
src/Scene
src/View
src/World
src/MultiSelect
)
if(MINGW)
	message("Building project for MinGW.")
	set(CMAKE_LIBRARY_PATH "C:/MinGW/lib")
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" ".la" ".dll" ".dll.a" ".lib")
	find_library(SDL2_LIB SDL2)
	find_library(SDL2_IMAGE_LIB SDL2_image)
	find_library(OPENGL opengl32)
	find_library(BOOST_SYSTEM boost_system)
	find_library(BOOST_FILESYSTEM boost_filesystem)
endif()
if(APPLE)
	message("Building project for OS X")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/System/Library/Frameworks/OpenGL.framework/Headers -I/Library/Frameworks/SDL2.framework/Headers -I/Library/Frameworks/SDL2_image.framework/Headers -I/Library/Frameworks/SDL2_ttf.framework/Headers")
	find_library(OPENGL OpenGL)
	find_library(SDL2_LIB SDL2)
	find_library(SDL2_IMAGE_LIB SDL2_image)
	find_library(BOOST_SYSTEM boost_system)
	find_library(BOOST_FILESYSTEM boost_filesystem)
endif()
if(NOT OPENGL)
	message(FATAL_ERROR "OpenGL library not found!")
endif()
if(NOT BOOST_FILESYSTEM)
	message(FATAL_ERROR "libboost_filesystem not found!")
endif()
if(NOT BOOST_SYSTEM)
	message(FATAL_ERROR "libboost_system not found!")
endif()
if(NOT SDL2_IMAGE_LIB)
	message(FATAL_ERROR "SDL2_image library not found!")
endif()
if(NOT SDL2_LIB)
	message(FATAL_ERROR "SDL2 library not found!")
endif()
include_directories (/usr/local/include)
include_directories (/usr/include)
include_directories (include)

target_link_libraries (Magnate
${OPENGL}
${SDL2_LIB}
${SDL2_IMAGE_LIB}
${BOOST_SYSTEM}
${BOOST_FILESYSTEM})

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
include/Draggable
include/SaveManager
include/Field
include/Label
include/Minimap
include/Model
include/Scene
include/Terrain
include/TerrainGen
include/View
include/World
include/MultiSelect
include/RenderRoutines
include/Renderer
include/UIRenderer
include/WorldRenderer
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
src/Draggable
src/SaveManager
src/Field
src/Label
src/Minimap
src/Model
src/Scene
src/Terrain
src/TerrainGen
src/View
src/World
src/MultiSelect
src/RenderRoutines
src/Renderer
src/UIRenderer
src/WorldRenderer
)
if(MINGW)
	message("Building project for MinGW.")
	set_target_properties(Magnate PROPERTIES LINK_SEARCH_START_STATIC 1)
	set_target_properties(Magnate PROPERTIES LINK_SEARCH_END_STATIC 1)
	set(CMAKE_LIBRARY_PATH "C:/MinGW/lib")
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" ".la" ".dll" ".dll.a" ".lib")
	include_directories(C:/MinGW/include/GL)
	include_directories(C:/MinGW/include/SDL2)
	include_directories(C:/MinGW/include/boost)
	find_library(MINGWLIB mingw32)
	find_library(SDL2_LIB SDL2)
	find_library(SDL2_MAIN_LIB SDL2main)
	find_library(SDL2_IMAGE_LIB SDL2_image)
	find_library(MLIB m)
	find_library(DINPUT_LIB dinput8)
	find_library(DXGUID_LIB dxguid)
	find_library(DXERR_LIB dxerr8)
	find_library(USER_LIB user32)
	find_library(GDI_LIB gdi32)
	find_library(WINMM_LIB winmm)
	find_library(IMM_LIB imm32)
	find_library(OLE_LIB ole32)
	find_library(OLEAUT_LIB oleaut32)
	find_library(SHELL_LIB shell32)
	find_library(VERSION_LIB version)
	find_library(UUID_LIB uuid)
	find_library(OPENGL opengl32)
	find_library(BOOST_SYSTEM boost_system)
	find_library(BOOST_FILESYSTEM boost_filesystem)
	target_link_libraries (Magnate
		${MINGWLIB}
		${SDL2_MAIN_LIB}
		${SDL2_LIB})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
	target_link_libraries (Magnate
		${SDL2_IMAGE_LIB}
		${MLIB}
		${DINPUT_LIB}
		${DXGUID_LIB}
		${DXERR_LIB}
		${USER_LIB}
		${GDI_LIB}
		${WINMM_LIB}
		${IMM_LIB}
		${OLE_LIB}
		${OLEAUT_LIB}
		${SHELL_LIB}
		${VERSION_LIB}
		${UUID_LIB}
		${OPENGL}
		${BOOST_SYSTEM}
		${BOOST_FILESYSTEM})

endif()
if(APPLE)
	message("Building project for OS X")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/System/Library/Frameworks/OpenGL.framework/Headers -I/Library/Frameworks/SDL2.framework/Headers -I/Library/Frameworks/SDL2_image.framework/Headers -I/Library/Frameworks/SDL2_ttf.framework/Headers")
	find_library(OPENGL OpenGL)
	find_library(SDL2_LIB SDL2)
	find_library(SDL2_IMAGE_LIB SDL2_image)
	find_library(BOOST_SYSTEM boost_system)
	find_library(BOOST_FILESYSTEM boost_filesystem)
	target_link_libraries (Magnate
		${OPENGL}
		${SDL2_LIB}
		${SDL2_IMAGE_LIB}
		${BOOST_SYSTEM}
		${BOOST_FILESYSTEM})
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

if(MINGW)
		set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif()

cmake_minimum_required (VERSION 3.0)
project (Magnate)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -F/Library/Frameworks -F/System/Library/Frameworks -lOpenGL -lSDL2 -lSDL2_image -lSDL2_ttf -I/System/Library/Frameworks/OpenGL.framework/Headers -I/Library/Frameworks/SDL2.framework/Headers -I/Library/Frameworks/SDL2_image.framework/Headers -I/Library/Frameworks/SDL2_ttf.framework/Headers")

#Have to handle some libraries a certain way because they are .frameworks
if(apple)
	find_path (GL_INCLUDE_DIR gl.h)
	find_path (SDL2_INCLUDE_DIR SDL.h)
	find_path (SDL2_IMAGE_INCLUDE_DIR SDL_image.h)
	find_path (SDL2_TTF_INCLUDE_DIR SDL_ttf.h)
	set (EXTRA_LIBS OpenGL SDL2 SDL2_image SDL2_ttf)
endif(apple)

include_directories (/usr/local/include)
include_directories (/usr/include)
include_directories (include)

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
)

target_link_libraries (Magnate 
boost_filesystem

)

if(apple)
	target_link_libraries (Magnate ${EXTRA_LIBS})
endif(apple)
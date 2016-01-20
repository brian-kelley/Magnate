#ifndef __SDL_HEADERS__
#define __SDL_HEADERS__

#ifdef __APPLE__

#include "SDL.h"
#include "SDL_image.h"
#include "gl.h"

#elif __linux

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/gl.h>

#elif _WIN32

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/gl.h>

#endif //platforms

#endif
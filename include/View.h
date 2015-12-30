//
//  View.h
//  MagIndev
//
//  Created by Brian Kelley on 10/20/14293.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __MagIndev__View__
#define __MagIndev__View__

#ifdef __APPLE__
#include "SDL.h"
#include "SDL_ttf.h"
#include "gl.h"
#include <boost/filesystem.hpp>
#elif __linux
#include <SDL2/SDL.h>
#include <GL/gl.h>
#elif _WIN32
#include <SDL2\SDL.h>
#include <GL/gl.h>
#endif
#include <iostream>
#include <vector>
#include "GUI.h"
#include "UIRenderer.h"
#include "RenderRoutines.h"
#include "Model.h"
#include "Atlas.h"
#include "Building.h"
#include "Coord.h"
#include "Constants.h"
#include "Renderer.h"
#include "TexManager.h"

class View
{
public:
    View();
    ~View();
    void prepareFrame();
    void finalizeFrame();
    void updateWindowSize();
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_GLContext context;
    Atlas atlas;
};

#endif
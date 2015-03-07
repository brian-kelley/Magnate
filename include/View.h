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
#include "UIRenderer.h"
#include "RenderRoutines.h"
#include "Model.h"
#include "Atlas.h"
#include "Building.h"
#include "Cuboid.h"
#include "Scene.h"
#include "Button.h"
#include "Label.h"
#include "Field.h"
#include "MultiSelect.h"
#include "Coord.h"
#include "Constants.h"
#include "Terrain.h"

namespace view
{
    //Private data members
    extern SDL_Window* window;
    extern SDL_Renderer* renderer;
    extern SDL_GLContext context;
    extern Atlas* mainAtlas;
    //Publicly used functions
    extern void init();
    extern void dispose();
    extern void updateWindowSize();
    extern void drawWorld(World& currentWorld);
    extern void prepareFrame();
    extern void finalizeFrame();
    //Privately used functions
    extern void frame();
    extern void configGL();
    extern void initSDLVideo();
    extern void drawBuilding(Building& b);
}

#endif
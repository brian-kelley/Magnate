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
#include <SDL2/SDL.h>
#include <gl.h>
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
#include "Model.h"
#include "Atlas.h"
#include "Building.h"
#include "Cuboid.h"
#include "Scene.h"
#include "Button.h"
#include "Label.h"
#include "Field.h"
#include "Coord.h"
#include "Constants.h"

namespace view
{
    //Private data members
    extern int scrX;
    extern int scrY;
    extern SDL_Window* window;
    extern SDL_Renderer* renderer;
    extern SDL_GLContext context;
    extern Atlas* mainAtlas;
    //Public functions
    extern void init(int screenX = 0, int screenY = 0);
    extern void dispose();
    extern void updateWindowSize();
    extern void drawWorld(World* currentWorld);
    extern void drawScene(Scene& s);
    extern void prepareFrame();
    extern void finalizeFrame();
    //Private functions
    extern void blit(int tex, int x, int y);
    extern void blit(int tex, int x1, int y1, int x2, int y2);
    extern void frame();
    extern void configGL();
    extern void initSDLVideo();
    extern void initAtlas();
    extern void drawCuboid(Cuboid& c);
    extern void drawBuilding(Building& b);
    extern void drawButton(Button& b);
    extern void drawLabel(Label& l);
    extern void drawField(Field& f);
    extern void drawScrollBlock(ScrollBlock& sb);
    extern void drawString(std::string text, int x, int y);
    extern void drawString(std::string text, int x, int y, float scale);
    extern void drawString(std::string text, int x, int y, float scale, float r, float g, float b);
}

#endif

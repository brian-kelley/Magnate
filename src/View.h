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
#elif __linux
#include <SDL2/SDL.h>
#include <GL/gl.h>
#elif _WIN32
#include <SDL2\SDL.h>
#include <GL\gl.h>
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

class View
{
public:
    View(int screenX = 0, int screenY = 0);
    ~View();
    void updateWindowSize();
    void drawWorld(World* currentWorld);
    void drawScene(Scene& s);
    void prepareFrame();
    void finalizeFrame();
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_GLContext context;
    Atlas* mainAtlas;
    void blit(int tex, int x, int y);
    void blit(int tex, int x1, int y1, int x2, int y2);
    void frame();
    void configGL();
    void initSDLVideo();
    void initAtlas();
    void drawCuboid(Cuboid& c);
    void drawBuilding(Building& b);
    void drawButton(Button& b);
    void drawLabel(Label& l);
    void drawField(Field& f);
    void drawScrollBlock(ScrollBlock& sb);
    void drawString(std::string text, int x, int y);
    void drawString(std::string text, int x, int y, float scale);
    void drawString(std::string text, int x, int y, float scale, float r, float g, float b);
    int scrX;
    int scrY;
};

#endif

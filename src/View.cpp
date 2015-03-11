//
//  View.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/20/14293.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "View.h"

using namespace std;
using namespace constants;
using namespace view;
using namespace boost::filesystem;
using namespace RenderRoutines;

SDL_Window* view::window;
SDL_Renderer* view::renderer;
SDL_GLContext view::context;
Atlas* view::mainAtlas;

void view::init()
{
    //Once a world is loaded these may be set again
    constants::screenX = 0;
    constants::screenY = 0;
    initSDLVideo();
    configGL();
    RenderRoutines::initAtlas(renderer);
}

void view::dispose()
{
    delete mainAtlas;
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    SDL_DestroyWindow(window);
    window = nullptr;
    IMG_Quit();
    SDL_Quit();
}

void view::prepareFrame()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void view::finalizeFrame()
{
    SDL_GL_SwapWindow(window);
}

void view::drawBuilding(Building& b)
{
    //b's cuboids are already in correct order for rendering (back to front)
    for(int i = 0; i < b.numCuboids(); i++)
    {
        drawCuboid(b.getCuboidRef(i));
    }
}

void view::drawWorld(World& currentWorld)   //probably too general of a function
{

}

void view::configGL()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(glGetError() != GL_NO_ERROR)
    {
        cout << "Couldn't initialize GL_PROJECTION mode!" << endl;
        exit(3);
    }
    glOrtho(0, constants::WINDOW_W, constants::WINDOW_H, 0, 1, -1);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_TEXTURE_2D);
    glClearColor(1, 1, 1, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void view::updateWindowSize()
{
    SDL_GetWindowSize(window, &constants::WINDOW_W, &constants::WINDOW_H);
}

void view::initSDLVideo()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        cout << "Failed to SDL video." << endl;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    window = SDL_CreateWindow("Magnate", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              constants::WINDOW_W, constants::WINDOW_H, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if(!window)
    {
        cout << "Failed to create SDL window." << endl;
        exit(4);
    }
    context = SDL_GL_CreateContext(window);
    if(context == nullptr)
    {
        cout << "Error creating GL context." << endl;
        exit(5);
    }
    SDL_GL_SetSwapInterval(1);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

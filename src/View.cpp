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

void view::init()
{
    //Once a world is loaded these may be set again
    initSDLVideo();
    configGL();
    RenderRoutines::initAtlas(renderer);
    Renderer::init();
}

void view::dispose()
{
    Renderer::dispose();
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
    Renderer::startFrame();
}

void view::finalizeFrame()
{
    Renderer::endFrame();
    SDL_GL_SwapWindow(window);
}

void view::drawBuilding(Building& b)
{
    //b's cuboids are already in correct order for rendering (back to front)
    for(int i = 0; i < b.numCuboids(); i++)
    {
        drawCuboid(b.getCuboid(i));
    }
}

void view::drawWorld()   //probably too general of a function
{

}

void view::configGL()
{
    glClearColor(1, 1, 1, 1);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void view::updateWindowSize()
{
    SDL_GetWindowSize(window, &constants::WINDOW_W, &constants::WINDOW_H);
}

void view::initSDLVideo()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
        cout << "Failed to SDL video." << endl;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    window = SDL_CreateWindow("Magnate", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              constants::WINDOW_W, constants::WINDOW_H, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if(!window)
    {
        cout << "Failed to create SDL window." << endl;
        exit(-1);
    }
    context = SDL_GL_CreateContext(window);
    if(context == nullptr)
    {
        cout << SDL_GetError() << endl;
        cout << "Error creating GL context." << endl;
        exit(-1);
    }
    SDL_GL_SetSwapInterval(1);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    GLERR
}

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
using namespace boost::filesystem;
using namespace RenderRoutines;

View::View()
{
    //Initialize SDL window
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
    //Initialize the atlas
    atlas = Atlas("main", renderer);
    //Configure OpenGL
    glClearColor(1, 1, 1, 1);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Renderer::init();
    TexManager::init();
}

View::~View()
{
    Renderer::dispose();
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    SDL_DestroyWindow(window);
    window = nullptr;
    IMG_Quit();
    SDL_Quit();
}

void View::prepareFrame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Renderer::startFrame();
}

void View::finalizeFrame()
{
    Renderer::endFrame();
    SDL_GL_SwapWindow(window);
}

void View::updateWindowSize()
{
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    GUI::processResize(w, h);
}

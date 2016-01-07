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

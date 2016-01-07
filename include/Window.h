#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <iostream>
#include "GenTypes.h"
#include "DebugTools.h"
#include "SdlHeaders.h"

class Window
{
public:
    Window(int w, int h);
    SDL_Renderer* getRenderer();
private:
    void configGL();
    SDL_Window* sdlWindow;
    SDL_Renderer* sdlRenderer;
    SDL_GLContext glContext;
};

#endif
#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <iostream>
#include "GUI.h"
#include "GenTypes.h"
#include "DebugTools.h"
#include "SdlHeaders.h"
#include "Atlas.h"

enum struct WindowMode
{
    WINDOW,                 //window that can be any size
    WINDOWED_FULLSCREEN,    //window that looks like fullscreen
    FULLSCREEN              //actual fullscreen
};

namespace Window
{
    void init(int w, int h);
    void dispose();
    Pos2 getSize();
    SDL_Renderer* getRenderer();
    void prepareFrame();
    void endFrame();
    void resize(int w, int h);
    void toggleFullscreen(WindowMode wm);
    void configGL();
    extern int w;
    extern int h;
    extern SDL_Window* sdlWindow;
    extern SDL_Renderer* sdlRenderer;
    extern SDL_GLContext glContext;
};

#endif

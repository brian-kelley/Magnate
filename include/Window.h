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

class Window
{
public:
    Window(int width, int height);
    ~Window();
    Pos2 getSize();
    SDL_Renderer* getRenderer();
    void prepareFrame();
    void endFrame();
    void resize(int w, int h);
    void toggleFullscreen(WindowMode wm);
private:
    void configGL();
    int w;
    int h;
    SDL_Window* sdlWindow;
    SDL_Renderer* sdlRenderer;
    SDL_GLContext glContext;
};

#endif

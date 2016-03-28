#ifndef __GAME_H__
#define __GAME_H__

#include <iostream>
#include <vector>
#include <chrono>
#include <ctime>
#include "GenTypes.h"
#include "GUI.h"
#include "SdlHeaders.h"
#include "FileIO.h"
#include "Renderer.h"
#include "World.h"
#include "Camera.h"
#include "Menus.h"

namespace Game
{
    void init();
    void mainLoop();
    void update();
    void debugInit();
    void processWindow(void* inst, const SDL_WindowEvent& event);
    void processMisc(void* inst, const SDL_EventType& event);
    void processMenuEvent(void* inst, const GeneralMsg& msg);
    extern bool terminating;        //whether the program will terminate after this frame
};

#endif

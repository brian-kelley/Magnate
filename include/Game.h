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

class Game
{
public:
    Game();
private:
    Renderer renderer;
    Scene mainMenu;
    Scene saveMenu;
    Scene gameScene;
    void mainLoop();
    void update();
    bool terminating;        //whether the program will terminate after this frame
    static void processWindow(void* inst, const SDL_WindowEvent& event);
    static void processMisc(void* inst, const SDL_EventType& event);
    static void processMenuEvent(void* inst, const GeneralMsg& msg);
};

#endif
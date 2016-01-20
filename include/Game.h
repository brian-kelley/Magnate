#ifndef __GAME_H__
#define __GAME_H__

#include <iostream>
#include <vector>
#include <chrono>
#include "GenTypes.h"
#include "GUI.h"
#include "SdlHeaders.h"
#include "FileIO.h"
#include "Renderer.h"
#include "World.h"
#include "Atlas.h"
#include "Input.h"

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
    //gui
    void initScenes();       //create all widgets
    static void mainStart(void* inst, void*);
    static void mainQuit(void* inst, void*);
    static void processWindow(void* inst, const SDL_WindowEvent& event);
    static void processMisc(void* inst, const SDL_EventType& event);
};

#endif
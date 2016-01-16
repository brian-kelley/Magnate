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

class Game
{
public:
    Game();
private:
    Renderer renderer;
    void mainLoop();
    void update();
    bool terminating;        //whether the program will terminate after this frame
};

#endif
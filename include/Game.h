#ifndef __CONTROL_H__
#define __CONTROL_H__

#include <cstdint>
#include <iostream>
#include <vector>
#include <chrono>
#include <boost/filesystem.hpp>
#include "View.h"
#include "Model.h"
#include "Constants.h"
#include "GenTypes.h"
#include "GUI.h"
#include "SdlHeaders.h"

class Game
{
public:
    Game();
private:
    World world;
    Renderer renderer;
    void mainLoop();
    void update();
    GameState state;
    bool terminating;        //whether the program will terminate after this frame
};

#endif
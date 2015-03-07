//
//  main.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/20/14293.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <boost/filesystem.hpp>
#include <stdlib.h>
#include <time.h>
#include "Control.h"
#include "Button.h"

using namespace std;
using namespace boost::filesystem;

int main(int argc, char** argv)
{
    srand(time(NULL));
    Control::init();
    int ticks;
    while(true)
    {
        ticks = SDL_GetTicks();
        Control::update();
        if(Control::isTerminating())
        {
            break;
        }
        int delay = 16 - (SDL_GetTicks() - ticks);
        if(delay > 0)
        {
            SDL_Delay(delay);
        }
    }
    Control::dispose();
    return 0;
}

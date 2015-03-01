//
//  Control.h
//  MagIndev
//
//  Created by Brian Kelley on 10/21/14294.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __MagIndev__Control__
#define __MagIndev__Control__

#ifdef __APPLE__
#include "SDL.h"
#include <boost/filesystem.hpp>
#elif __linux
#include <SDL2/SDL.h>
#elif _WIN32
#include <SDL2\SDL.h>
#endif
#include <stdio.h>
#include <cstdint>
#include <iostream>
#include <vector>
#include "View.h"
#include "Component.h"
#include "Button.h"
#include "Field.h"
#include "Label.h"
#include "Scene.h"
#include "Constants.h"
#include "SaveManager.h"

namespace Control
{
    extern void init();
    extern void dispose();
    extern void update();
    extern bool isTerminating();
    enum SNAME
    {
        MAIN_MENU,
        SAVE_MENU,
        GAME,
        PAUSE
    };
    extern int oldWindowW;
    extern int oldWindowH;
    extern std::map<SNAME, Scene*> scenes;
    extern SDL_Event* currentEvent;
    extern Scene* currentScene;
    extern void processWindowResize();
    extern void processWindowEvent(SDL_Event& e);
    extern void processMouseMotionEvent();
    extern void processMouseButtonEvent(SDL_Event& e);
    extern void processMouseWheelEvent(SDL_Event& e);
    extern void processKeyboardEvent(SDL_Event& e);
    extern void updateUISize();
    extern bool terminating;
    extern bool updatingView;
    extern bool trackingMouse;
    extern bool trackingKeyboard;
    extern void initScenes();
    extern void clearEnables();
}

#endif
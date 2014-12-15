//
//  Control.h
//  MagIndev
//
//  Created by Brian Kelley on 10/21/14294.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __MagIndev__Control__
#define __MagIndev__Control__

#include <SDL2/SDL.h>
#include <stdio.h>
#include <cstdint>
#include <iostream>
#include <vector>
#include "View.h"
#include "Button.h"
#include "Field.h"
#include "Label.h"
#include "Scene.h"
#include "DirManager.h"

namespace Control
{
    extern void init();
    extern void dispose();
    extern void update();
    extern bool isTerminating();
    extern View* view;
    extern Model* model;
    enum SNAME
    {
        MAIN_MENU,
        SAVE_MENU,
        GAME,
        PAUSE
    };
    extern SNAME current;
    extern std::vector<std::string> saveNames;
    extern int mouseX;
    extern int mouseY;
    extern int oldWindowW;
    extern int oldWindowH;
    extern std::vector<Scene*> scenes;
    extern SDL_Event* currentEvent;
    extern void processWindowResize();
    extern void processWindowEvent(SDL_Event& e);
    extern void processMouseMotionEvent(SDL_Event& e);
    extern void processMouseButtonEvent(SDL_Event& e);
    extern void processMouseWheelEvent(SDL_Event& e);
    extern void processKeyboardEvent(SDL_Event& e);
    extern void updateUISize();
    extern bool terminating;
    extern bool updatingView;
    extern bool trackingMouse;
    extern bool trackingKeyboard;
    extern void initScenes();
    //Now list callback functions all UI components
    extern void mainQuitButton();
    extern void mainStartButton();
    extern void saveBackButton();
    extern void saveNameUpdate();
    extern void loadSave();
}

#endif

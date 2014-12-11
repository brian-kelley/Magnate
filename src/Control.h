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

class Control
{
public:
    Control();
    ~Control();
    void update();
    bool isTerminating();
private:
    View* view;
    Model* model;
    enum SNAME
    {
        MAIN_MENU,
        SAVE_MENU
    };
    SNAME current;
    int mouseX;
    int mouseY;
    int oldWindowW;
    int oldWindowH;
    std::vector<Scene*> scenes;
    SDL_Event* currentEvent;
    void processWindowResize();
    void processWindowEvent(SDL_Event& e);
    void processMouseMotionEvent(SDL_Event& e);
    void processMouseButtonEvent(SDL_Event& e);
    void processMouseWheelEvent(SDL_Event& e);
    void processKeyboardEvent(SDL_Event& e);
    void updateUISize();
    bool terminating;
    bool updatingView;
    bool trackingMouse;
    bool trackingKeyboard;
    void initScenes();
    //Now list callback functions all UI components
    static void mainQuitButton();
    static void mainStartButton();
    static void saveBackButton();
    static void saveName1Update();
    static void saveName2Update();
    static void saveName3Update();
    static void saveName4Update();
    static void saveStart1();
    static void saveStart2();
    static void saveStart3();
    static void saveStart4();
};

#endif

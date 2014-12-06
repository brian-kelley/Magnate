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
    Button* test;
    void update();
    bool isTerminating();
private:
    View* view;
    Model* model;
    int scene;
    int mouseX;
    int mouseY;
    int oldWindowW;
    int oldWindowH;
    std::vector<Scene*>* scenes;
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
};

#endif

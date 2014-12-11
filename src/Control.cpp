//
//  Control.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/21/14294.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Control.h"

using namespace std;

Control::Control()
{
    //Populate vector of Scenes from file
    view = new View();
    model = new Model();
    this->oldWindowW = constants::WINDOW_W;
    this->oldWindowH = constants::WINDOW_H;
    SDL_GetMouseState(&this->mouseX, &this->mouseY);
    terminating = false;
    updatingView = true;
    trackingMouse = true;
    trackingKeyboard = true;
    this->initScenes();
    this->currentEvent = new SDL_Event();
    this->current = MAIN_MENU;
}

Control::~Control()
{
    delete view;
    view = nullptr;
    for(int i = 0; i < (int) this->scenes.size(); i++)
    {
        delete scenes[i];
    }
    this->scenes.clear();
    delete this->currentEvent;
}

void Control::update()
{
    this->oldWindowW = constants::WINDOW_W;
    this->oldWindowH = constants::WINDOW_H;
    SDL_PumpEvents();
    while(SDL_PollEvent(currentEvent))
    {
        switch(currentEvent->type)
        {
            case SDL_KEYDOWN:
                if(trackingKeyboard)
                {
                    processKeyboardEvent(*currentEvent);
                }
                break;
            case SDL_KEYUP:
                if(trackingKeyboard)
                {
                    processKeyboardEvent(*currentEvent);
                }
                break;
            case SDL_TEXTINPUT:
                if(trackingKeyboard)
                {
                    processKeyboardEvent(*currentEvent);
                }
                break;
            case SDL_WINDOWEVENT:
                processWindowEvent(*currentEvent);
                break;
            case SDL_MOUSEBUTTONDOWN:
                if(trackingMouse)
                {
                    processMouseButtonEvent(*currentEvent);
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if(trackingMouse)
                {
                    processMouseButtonEvent(*currentEvent);
                }
                break;
            case SDL_MOUSEMOTION:
                if(trackingMouse)
                {
                    processMouseMotionEvent(*currentEvent);
                }
                break;
            case SDL_MOUSEWHEEL:
                if(trackingMouse)
                {
                    processMouseWheelEvent(*currentEvent);
                }
                break;
            case SDL_EventType::SDL_QUIT:
                terminating = true;
                break;
        }
    }
    view->update();
}

bool Control::isTerminating()
{
    return terminating;
}

void Control::processKeyboardEvent(SDL_Event &e)
{

}

void Control::processMouseButtonEvent(SDL_Event &e)
{

}

void Control::processMouseMotionEvent(SDL_Event &e)
{
    Scene* scenePtr = this->scenes[current];
    int x = (int) e.motion.x;
    int y = (int) e.motion.y;
    cout << x << " " << y << endl;
    for(int i = 0; i < (int) scenePtr->getButtons()->size(); i++)
    {

    }
}

void Control::processMouseWheelEvent(SDL_Event &e)
{
    int delta = e.wheel.y;
    cout << "Scrolled by " << delta << " units." << endl;
}

void Control::processWindowEvent(SDL_Event &e)
{
    switch(e.window.event)
    {
        case SDL_WINDOWEVENT_CLOSE:
        	this->terminating = true;
            break;
        case SDL_WINDOWEVENT_ENTER: //mouse enters window
        	this->trackingMouse = true;
            break;
        case SDL_WINDOWEVENT_LEAVE: //mouse leaves window
        	this->trackingMouse = false;
        	break;
        case SDL_WINDOWEVENT_FOCUS_GAINED: //window gains keyboard focus
            this->trackingKeyboard = true;
        	break;
        case SDL_WINDOWEVENT_FOCUS_LOST: //window lost keyboard focus
            this->trackingKeyboard = false;
        	break;
        case SDL_WINDOWEVENT_HIDDEN: //window has been hidden
            this->updatingView = false;
        	break;
        case SDL_WINDOWEVENT_SHOWN: //window has been shown
        case SDL_WINDOWEVENT_MAXIMIZED: //update window size and UI layout
        case SDL_WINDOWEVENT_RESTORED: //window needs to be refreshed for some reason
        	this->updatingView = true;
        	break;
        case SDL_WINDOWEVENT_RESIZED: //window resized by user
        case SDL_WINDOWEVENT_SIZE_CHANGED: //system or API call changed window size
        	this->oldWindowW = constants::WINDOW_W;
        	this->oldWindowH = constants::WINDOW_H;
            view->updateWindowSize();
        	this->updateUISize();
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
            this->updatingView = false;
            break;
    }
}

void Control::updateUISize()
{

}

void Control::initScenes()
{
    Scene* stacker;
    /* Syntax for adding a new scene (Note: must correspond in order to SNAME elements!)
     stacker = new Scene();
     stacker->addButton(lolMyOnlyButtonPtr);
     this->scenes.push_back(stacker);
     */
    stacker = new Scene();
    stacker->addButton(new Button(200, 100, 240, 100, "Start Game", &mainStartButton));
    stacker->addButton(new Button(200, 240, 240, 100, "Quit Game", &mainQuitButton));
    this->scenes.push_back(stacker);
    stacker = new Scene();
    stacker->addButton(new Button(550, 50, 100, 80, "Load", &saveStart1));
    stacker->addButton(new Button(550, 100, 100, 80, "Load", &saveStart2));
    stacker->addButton(new Button(550, 150, 100, 80, "Load", &saveStart3));
    stacker->addButton(new Button(550, 200, 100, 80, "Load", &saveStart4));
    stacker->addField(new Field(250, 50, 400, 80, "", &saveName1Update));
    stacker->addField(new Field(250, 100, 400, 80, "", &saveName2Update));
    stacker->addField(new Field(250, 150, 400, 80, "", &saveName3Update));
    stacker->addField(new Field(250, 200, 400, 80, "", &saveName4Update));
    this->scenes.push_back(stacker);
}
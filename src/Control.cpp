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
    terminating = false;
    updatingView = true;
    trackingMouse = true;
    trackingKeyboard = true;
    this->scenes = DirManager::parseScenes();
    cout << "Parsed and loaded " << scenes->size() << " scenes." << endl;
    this->currentEvent = new SDL_Event();
    this->scene = 0;
}

Control::~Control()
{
    delete view;
    view = nullptr;
    delete test;
    for(int i = 0; i < (int) this->scenes->size(); i++)
    {
        delete (*scenes)[i];
        (*scenes)[i] = nullptr;
    }
    this->scenes->clear();
    delete scenes;
    this->scenes = nullptr;
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
    Scene* scenePtr = this->scenes->at(this->scene);
    int x = (int) e.motion.x;
    int y = (int) e.motion.y;
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
        	this->updateUISize();
            view->updateWindowSize();
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
            this->updatingView = false;
            break;
    }
}

//
//  Control.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/21/14294.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Control.h"

using namespace std;
using namespace Control;

SNAME Control::current;
bool Control::terminating;
bool Control::updatingView;
bool Control::trackingMouse;
bool Control::trackingKeyboard;
View* Control::view;
Model* Control::model;
int Control::oldWindowW;
int Control::oldWindowH;
int Control::mouseX;
int Control::mouseY;
SDL_Event* Control::currentEvent;
vector<Scene*> Control::scenes;
vector<string> Control::saveNames;

void Control::init()
{
    view = new View();
    oldWindowW = constants::WINDOW_W;
    oldWindowH = constants::WINDOW_H;
    SDL_GetMouseState(&mouseX, &mouseY);
    terminating = false;
    updatingView = true;
    trackingMouse = true;
    trackingKeyboard = true;
    saveNames = DirManager::listSaves();
    initScenes();
    currentEvent = new SDL_Event();
    current = MAIN_MENU;
    //saveNames = DirManager::listSaves();
}

void Control::dispose()
{
    delete view;
    view = nullptr;
    for(int i = 0; i < (int) scenes.size(); i++)
    {
        delete scenes[i];
    }
    scenes.clear();
    delete currentEvent;

}

void Control::update()
{
    oldWindowW = constants::WINDOW_W;
    oldWindowH = constants::WINDOW_H;
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
    view->prepareFrame();
    if(current == GAME)
    {
        view->drawWorld(model->getCurrentWorld());
    }
    view->drawScene(*scenes[current]);
    view->finalizeFrame();
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
    if(e.button.state == SDL_PRESSED)
    {
        Button* btnPtr;
        SDL_Rect* btnRect;
        for(int i = 0; i < (int) scenes[current]->getButtons().size(); i++)
        {
            btnPtr = &(scenes[current]->getButtons())[i];
            btnRect = &btnPtr->getRect();
            if(btnRect->x < mouseX && btnRect->x + btnRect->w > mouseX
               && btnRect->y < mouseY && btnRect->y + btnRect->h > mouseY)
            {
                (*(btnPtr->getCallback())) ();
                break;
            }
        }
    }
}

void Control::processMouseMotionEvent(SDL_Event &e)
{
    Scene* scenePtr = scenes[current];
    mouseX = (int) e.motion.x;
    mouseY = (int) e.motion.y;
    for(int i = 0; i < (int) scenePtr->getButtons().size(); i++)
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
        	terminating = true;
            break;
        case SDL_WINDOWEVENT_ENTER: //mouse enters window
        	trackingMouse = true;
            break;
        case SDL_WINDOWEVENT_LEAVE: //mouse leaves window
        	trackingMouse = false;
        	break;
        case SDL_WINDOWEVENT_FOCUS_GAINED: //window gains keyboard focus
            trackingKeyboard = true;
        	break;
        case SDL_WINDOWEVENT_FOCUS_LOST: //window lost keyboard focus
            trackingKeyboard = false;
        	break;
        case SDL_WINDOWEVENT_HIDDEN: //window has been hidden
            updatingView = false;
        	break;
        case SDL_WINDOWEVENT_SHOWN: //window has been shown
        case SDL_WINDOWEVENT_MAXIMIZED: //update window size and UI layout
        case SDL_WINDOWEVENT_RESTORED: //window needs to be refreshed for some reason
        	updatingView = true;
        	break;
        case SDL_WINDOWEVENT_RESIZED: //window resized by user
        case SDL_WINDOWEVENT_SIZE_CHANGED: //system or API call changed window size
        	oldWindowW = constants::WINDOW_W;
        	oldWindowH = constants::WINDOW_H;
            view->updateWindowSize();
        	updateUISize();
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
            updatingView = false;
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
    scenes.push_back(stacker);
    stacker = new Scene();
    ScrollBlock* saveList = new ScrollBlock(320, 220, 550, 400);
    for(int i = 0; i < (int) saveNames.size(); i++)
    {
        saveList->addField(*new Field(250, 50 + 50 * i, 400, 40, saveNames[i], &saveNameUpdate));
        saveList->addButton(*new Button(550, 50 + 50 * i, 100, 40, "Load", &loadSave));
    }
    stacker->addScrollBlock(saveList);
    stacker->addButton(new Button(320, 400, 300, 80, "Back", &saveBackButton));
    scenes.push_back(stacker);
}

void Control::mainQuitButton()
{
    terminating = true;
}

void Control::mainStartButton()
{
    current = SAVE_MENU;
}

void Control::saveBackButton()
{
    current = MAIN_MENU;
}

void Control::saveNameUpdate()
{
    
}

void Control::loadSave()
{
    
}
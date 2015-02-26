//
//  Control.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/21/14294.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Control.h"

using namespace std;
using namespace constants;
using namespace Control;
using namespace boost::filesystem;

Field* Control::currentField;
Scene* Control::currentScene;
bool Control::terminating;
bool Control::updatingView;
bool Control::trackingMouse;
bool Control::trackingKeyboard;
int Control::oldWindowW;
int Control::oldWindowH;
SDL_Event* Control::currentEvent;
map<SNAME, Scene*> Control::scenes;

namespace ui        //place for callbacks etc.
{
    void mainQuitButton(void* obj)
    {
        Control::terminating = true;
    }
    void mainStartButton(void* obj)
    {
        clearEnables();
        currentScene = scenes[SAVE_MENU];
    }
    void saveBackButton(void* obj)
    {
        clearEnables();
        currentScene = scenes[MAIN_MENU];
    }
    void saveGameButton(void* obj)
    {
        clearEnables();
        currentScene = scenes[GAME];
    }
}

void Control::init()
{
    view::init();
    SaveManager::init();
    initScenes();
    oldWindowW = constants::WINDOW_W;
    oldWindowH = constants::WINDOW_H;
    SDL_GetMouseState(&constants::mouseX, &constants::mouseY);
    terminating = false;
    updatingView = true;
    trackingMouse = true;
    trackingKeyboard = true;
    currentEvent = new SDL_Event();
    currentField = nullptr;
    currentScene = scenes[MAIN_MENU];
    model::init();
}

void Control::dispose()
{
    //main task here is to destroy all scenes and their subcomponents
    for(auto iter : scenes)
    {
        delete iter.second;
    }
    //immediately remove all the dangling pointers left over
    scenes.clear();
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
                    processMouseMotionEvent();
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
    view::prepareFrame();
    view::drawComponent(*currentScene);
    view::finalizeFrame();
}

bool Control::isTerminating()
{
    return terminating;
}

void Control::processKeyboardEvent(SDL_Event &e)
{
    //first, see if the event applies to a field
    if(currentField)
    {
        currentField->processKey(e);
    }
}

void Control::processMouseButtonEvent(SDL_Event &e)
{
    if(e.button.state == SDL_PRESSED && e.button.button == SDL_BUTTON_LEFT)
    {
        (currentScene)->processLeftClick();
    }
}

void Control::processMouseMotionEvent()
{
    SDL_GetMouseState(&mouseX, &mouseY);
    ((Component*) currentScene)->processMouseMotion();
}

void Control::processMouseWheelEvent(SDL_Event &e)
{
    ((Component*) currentScene)->processScroll(e.wheel);
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
        case SDL_WINDOWEVENT_MAXIMIZED: //update window size and UI layout
            view::updateWindowSize();
            //updateUISize();
            updatingView = true;
            break;
        case SDL_WINDOWEVENT_SHOWN: //window has been shown
        case SDL_WINDOWEVENT_RESTORED: //window needs to be refreshed for some reason
        	trackingMouse = true;
        	trackingKeyboard = true;
        	updatingView = true;
        	break;
        case SDL_WINDOWEVENT_RESIZED: //window resized by user
        case SDL_WINDOWEVENT_SIZE_CHANGED: //system or API call changed window size
        	oldWindowW = constants::WINDOW_W;
        	oldWindowH = constants::WINDOW_H;
            view::updateWindowSize();
            //updateUISize(); //not really a priority, implement this later
            break;
        case SDL_WINDOWEVENT_MINIMIZED: //save a bit of energy by pausing rendering
            updatingView = false;
            break;
    }
}

void Control::updateUISize()
{
    currentScene->processResize();
}

void Control::initScenes()
{
    /* Main menu */
    Scene* mainMenu = new Scene();
    new Button(320, 180, 240, 100, "Start Game", &ui::mainStartButton, mainMenu);
    new Button(320, 300, 240, 100, "Quit Game", &ui::mainQuitButton, mainMenu);
    scenes[MAIN_MENU] = mainMenu;
    SaveManager::initMenu(&ui::saveBackButton, &ui::saveGameButton);
    scenes[SAVE_MENU] = SaveManager::getScene();
}

void Control::clearEnables()      //clear currentField and button hovers in current scene
{
    for(Component* c : currentScene->getChildren())
    {
        c->deactivate();
    }
}
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
using namespace Renderer;
using namespace boost::filesystem;
using namespace Coord;

Scene* Control::currentScene;
bool Control::terminating;
bool Control::camUpdated;
bool Control::updatingView;
bool Control::trackingMouse;
bool Control::trackingKeyboard;
bool Control::drawingTopo;
int Control::oldWindowW;
int Control::oldWindowH;
SDL_Event* Control::currentEvent;
map<SNAME, Scene*> Control::scenes;
const Uint8* Control::keystate = NULL;

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
        //If you get here without errors, SaveManager has ptr
        //to the right world object to use for game
        clearEnables();
        currentScene = scenes[GAME];
        WorldRenderer::init();
    }
}

void Control::init()
{
    Minimap::initMM();
    Coord::initCoord();
    view::init();
    SaveManager::init(&currentScene, &ui::saveBackButton, &ui::saveGameButton);
    initScenes();
    oldWindowW = constants::WINDOW_W;
    oldWindowH = constants::WINDOW_H;
    SDL_GetMouseState(&constants::mouseX, &constants::mouseY);
    terminating = false;
    updatingView = true;
    trackingMouse = true;
    trackingKeyboard = true;
    drawingTopo = false;
    currentEvent = new SDL_Event();
#ifndef MAGNATE_DEBUG
    // Normal game, go through main & save menus
    currentScene = scenes[MAIN_MENU];
#else
    //Debug mode, jump into 'asdf' for quicker testing of actual gameplay
    currentScene = scenes[GAME];
    GLERR
    SaveManager::loadTestWorld();
    Topo::generateTopo();
    GLERR
    SaveManager::transitionToGame(nullptr);
    GLERR
#endif
    keystate = SDL_GetKeyboardState(NULL);
    model::init();
    //Trap mouse in window
    SDL_SetRelativeMouseMode(SDL_TRUE);
    GLERR
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
    view::dispose();
}

void Control::update()
{
    GLERR
    oldWindowW = constants::WINDOW_W;
    oldWindowH = constants::WINDOW_H;
    camUpdated = false;
    keystate = SDL_GetKeyboardState(NULL);
    SDL_PumpEvents();
    while(SDL_PollEvent(currentEvent))
    {
        switch(currentEvent->type)
        {
            case SDL_KEYDOWN:
                if(trackingKeyboard)
                {
                    processKeyEvent(*currentEvent);
                }
                break;
            case SDL_KEYUP:
                if(trackingKeyboard)
                {
                    processKeyEvent(*currentEvent);
                }
                break;
            case SDL_TEXTINPUT:
                if(trackingKeyboard)
                {
                    processKeyTypedEvent(*currentEvent);
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
    if(currentScene == scenes[GAME])
    {
        if(mouseX < 2 || keystate[SDL_SCANCODE_A])
        {
            Camera::camLeft();
            camUpdated = true;
        }
        if(mouseX > WINDOW_W - 3 || keystate[SDL_SCANCODE_D])
        {
            Camera::camRight();
            camUpdated = true;
        }
        if(mouseY < 2 || keystate[SDL_SCANCODE_W])
        {
            Camera::camFwd();
            camUpdated = true;
        }
        if(mouseY > WINDOW_H - 3 || keystate[SDL_SCANCODE_S])
        {
            Camera::camBack();
            camUpdated = true;
        }
        if(keystate[SDL_SCANCODE_Q])
        {
            Camera::camCCW();
            camUpdated = true;
        }
        if(keystate[SDL_SCANCODE_E])
        {
            Camera::camCW();
            camUpdated = true;
        }
        if(keystate[SDL_SCANCODE_T])
            drawingTopo = true;
        else
            drawingTopo = false;
    }
    view::prepareFrame();
    UIRenderer::drawComponent(*currentScene);
    if(currentScene == scenes[GAME])
    {
        Minimap::render();
        if(drawingTopo)
            Topo::drawTopo();
    }
    color3f(1, 1, 1);
    RenderRoutines::blit(Atlas::tileFromName("cursor"), mouseX, mouseY);
    if(camUpdated)
    {
        WorldRenderer::updateVBOChunks(); //this only expensive if cam moved into new chunk
        Renderer::update3DMatrices();
    }
    view::finalizeFrame();
}

bool Control::isTerminating()
{
    return terminating;
}

void Control::processKeyTypedEvent(SDL_Event &e)
{
    //If there is an active field, pass this event to it
    if(Field::currentField)
        Field::currentField->processKey(e);
}

void Control::processKeyEvent(SDL_Event &e)
{
    if(e.key.state == SDL_PRESSED)
    {
        if(currentScene == scenes[GAME])
        {
            
        }
    }
}

void Control::processMouseButtonEvent(SDL_Event &e)
{
    if(e.button.state == SDL_PRESSED && e.button.button == SDL_BUTTON_LEFT)
    {
        mouseDown = true;
        if(currentScene == scenes[GAME] && Minimap::mmIsMouseOver())
            Minimap::update();
        else
            currentScene->processLeftClick();
    }
    else if(e.button.state == SDL_RELEASED && e.button.button == SDL_BUTTON_LEFT)
    {
        mouseDown = false;
        if(Draggable::activeDrag)
            Draggable::activeDrag->deactivate();
    }
}

void Control::processMouseMotionEvent()
{
    SDL_GetMouseState(&mouseX, &mouseY);
    if(currentScene == scenes[GAME])
    {
        if(Minimap::mmIsMouseOver() && mouseDown)
        {
            Minimap::update();
            camUpdated = true;
        }
    }
    else
        currentScene->processMouseMotion();
}

void Control::processMouseWheelEvent(SDL_Event &e)
{
    currentScene->processScroll(e.wheel);
    if(currentScene == scenes[GAME])
    {
        if(e.wheel.y > 0)
        {
            Camera::camPos.y *= (1 - ZOOM_SPEED);
            camUpdated = true;
            //TODO: Prevent user from zooming in too close
        }
        else if(e.wheel.y < 0)
        {
            Camera::camPos.y *= (1 + ZOOM_SPEED);
            //if(camPos.y > MAX_CAM_HEIGHT)
            //camPos.y = MAX_CAM_HEIGHT;
            camUpdated = true;
        }
    }
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
    new Button(320, 200, 240, 100, "Start Game", ui::mainStartButton, mainMenu);
    new Button(320, 320, 240, 100, "Quit Game", ui::mainQuitButton, mainMenu);
    new Label(320, 90, 200, 100, "Magnate", mainMenu);
    scenes[MAIN_MENU] = mainMenu;
    scenes[SAVE_MENU] = SaveManager::getScene();
    Scene* gameOverlay = new Scene();
    scenes[GAME] = gameOverlay;
}

void Control::clearEnables()      //clear currentField and button hovers in current scene
{
    for(Component* c : currentScene->getChildren())
    {
        c->deactivate();
    }
}

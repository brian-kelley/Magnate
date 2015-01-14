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

SNAME Control::current;
Scene* Control::currentScene;
Field* Control::currentField;
bool Control::terminating;
bool Control::updatingView;
bool Control::trackingMouse;
bool Control::trackingKeyboard;
View Control::view;
Model Control::model;
int Control::oldWindowW;
int Control::oldWindowH;
SDL_Event* Control::currentEvent;
vector<Scene> Control::scenes;
vector<string> Control::saveNames;

void Control::init()
{
    oldWindowW = constants::WINDOW_W;
    oldWindowH = constants::WINDOW_H;
    SDL_GetMouseState(&constants::mouseX, &constants::mouseY);
    terminating = false;
    updatingView = true;
    trackingMouse = true;
    trackingKeyboard = true;
    saveNames = DirManager::listSaves();
    initScenes();
    currentEvent = new SDL_Event();
    current = MAIN_MENU;
    currentScene = &scenes[0];
    currentField = nullptr;
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
    view.prepareFrame();
    if(current == GAME)
    {
        view.drawWorld(model.getCurrentWorld());
    }
    view.drawScene(scenes[current]);
    view.finalizeFrame();
}

bool Control::isTerminating()
{
    return terminating;
}

void Control::processKeyboardEvent(SDL_Event &e)
{
    //first, see if the event applies to a field
}

void Control::processMouseButtonEvent(SDL_Event &e)
{
    //if click outside current field, deactivate it
    if(currentField && !componentHandler::mouseInside(currentField->getCompID()))
    {
        currentField->deactivate();
    }
    if(e.button.state == SDL_PRESSED)
    {
        Button* btnPtr;
        intRect_t* curRect;
        for(int i = 0; i < (int) scenes[current].getButtons().size(); i++)
        {
            btnPtr = &(scenes[current].getButtons())[i];
            curRect = &componentHandler::getCompIntRect(btnPtr->getCompID());
            if(curRect->x < mouseX && curRect->x + curRect->w > mouseX
               && curRect->y < mouseY && curRect->y + curRect->h > mouseY)
            {
                (*(btnPtr->getCallback())) ();
                break;
            }
        }
        Field* fieldPtr;
        for(int i = 0; i < int(scenes[current].getFields().size()); i++)
        {
            fieldPtr = &(scenes[current].getFields())[i];
            curRect = &componentHandler::getCompIntRect(fieldPtr->getCompID());
            if(curRect->x < mouseX && curRect->x + curRect->w > mouseX
               && curRect->y < mouseY && curRect->y + curRect->h > mouseY)
            {
                {
                    currentField = fieldPtr;
                    currentField->activate();
                }
            }
        }
    }
}

void Control::processMouseMotionEvent(SDL_Event &e)
{
    Scene* scenePtr = &scenes[current];
    SDL_GetMouseState(&mouseX, &mouseY);
    intRect_t* btnRectPtr;
    Button* btnPtr;
    for(int i = 0; i < (int) scenePtr->getButtons().size(); i++)
    {
        btnRectPtr = &componentHandler::getCompIntRect(scenePtr->getButtons()[i].getCompID());
        if(btnRectPtr->x <= mouseX && btnRectPtr->x + btnRectPtr->w > mouseX
           && btnRectPtr->y <= mouseY && btnRectPtr->y + btnRectPtr->h > mouseY)
        {
            btnPtr->setHover(true);
        }
        else
        {
            btnPtr->setHover(false);
        }
    }
}

void Control::processMouseWheelEvent(SDL_Event &e)
{
    ScrollBlock* sbptr;
    intRect_t* rectptr;
    //process all scrollblocks in the screen with the scroll amount
    for(int i = 0; i < (int) currentScene->getScrollBlocks().size(); i++)
    {
        sbptr = &currentScene->getScrollBlocks()[i];
        //check if mouse inside the scroll block
        rectptr = &componentHandler::getCompIntRect(sbptr->getCompID());
        if(rectptr->x <= mouseX && rectptr->x + rectptr->w < mouseX
        	&& rectptr->y <= mouseY && rectptr->y + rectptr->h < mouseY)
        {
                sbptr->processScrollEvent(e.wheel);
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
            view.updateWindowSize();
            updateUISize();
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
            view.updateWindowSize();
            //updateUISize(); //not really a priority, implement this later
            break;
        case SDL_WINDOWEVENT_MINIMIZED: //save a bit of energy by pausing rendering
            updatingView = false;
            break;
    }
}

void Control::updateUISize()
{
    for(int j = 0; j < (int) currentScene->getButtons().size(); j++)
    {
        componentHandler::updateSize(currentScene->getButtons()[j].getCompID());
    }
    for(int j = 0; j < (int) currentScene->getFields().size(); j++)
    {
        componentHandler::updateSize(currentScene->getButtons()[j].getCompID());
    }
    for(int j = 0; j < (int) currentScene->getLabels().size(); j++)
    {
        componentHandler::updateSize(currentScene->getButtons()[j].getCompID());
    }
    for(int i = 0; i < (int) currentScene->getScrollBlocks().size(); i++)
    {
        currentScene->getScrollBlocks()[i].updateSize();
    }
}

void Control::initScenes()
{
    /* Main menu */
    Scene mainMenu;
    Button startGame(200, 100, 240, 100, "Start Game", &mainStartButton);
    mainMenu.addButton(startGame);
    Button quitGame(200, 240, 240, 100, "Quit Game", &mainQuitButton);
    mainMenu.addButton(quitGame);
    /* Save menu */
    Scene saveMenu;
    scenes.push_back(mainMenu);
    int numSaves = (int) saveNames.size();
    ScrollBlock saveList(320, 220, 550, 400, 550, 90 + 50 * numSaves + BORDER_WIDTH);
    for(int i = 0; i < numSaves; i++)
    {
        Field nameField(250, 50 + 50 * i, 400, 40, saveNames[i], &saveNameUpdate);
        saveList.addField(nameField);
        Button nameButton(550, 50 + 50 * i, 100, 40, "Load", &loadSave);
        saveList.addButton(nameButton);
    }
    Field newNameField(250, 50 + 50 * numSaves, 400, 40, "", &newSaveNameUpdate);
    saveList.addField(newNameField);
    Button newNameButton(550, 50 + 50 * numSaves, 100, 40, "Create", &newSaveCreate);
    saveList.addButton(newNameButton);
    saveMenu.addScrollBlock(saveList);
    Button backToMain(320, 400, 300, 80, "Back", &saveBackButton);
    saveMenu.addButton(backToMain);
    scenes.push_back(saveMenu);
}

void Control::mainQuitButton()
{
    terminating = true;
}

void Control::mainStartButton()
{
    currentScene = &scenes[SNAME::SAVE_MENU];
}

void Control::saveBackButton()
{
    currentScene = &scenes[SNAME::MAIN_MENU];
}

void Control::saveNameUpdate(std::string name)
{
    
}

void Control::newSaveNameUpdate(std::string name)
{
    
}

void Control::loadSave()
{
    
}

void Control::newSaveCreate()
{
    //call something that generates world, creates folder and files in dir/saves/
}

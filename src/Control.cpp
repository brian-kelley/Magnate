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
SaveManager* Control::sman;
bool Control::terminating;
bool Control::updatingView;
bool Control::trackingMouse;
bool Control::trackingKeyboard;
int Control::oldWindowW;
int Control::oldWindowH;
SDL_Event* Control::currentEvent;
map<SNAME, Scene> Control::scenes;

namespace ui        //place for callbacks etc.
{
    void mainQuitButton(int compID)
    {
        Control::terminating = true;
    }
    void mainStartButton(int compID)
    {
        clearEnables();
        currentScene = &scenes[SAVE_MENU];
    }
    void saveBackButton(int compID)
    {
        clearEnables();
        currentScene = &scenes[MAIN_MENU];
    }
    void saveNameUpdate(int compID)
    {
        
    }
    void newSaveNameUpdate(int compID)
    {
        
    }
    void newSaveCreate(int compID)
    {
        
    }
    void loadSave(int compID)
    {
        
    }
}

void Control::init()
{
    view::init();
    sman = new SaveManager();
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
    currentScene = &scenes[MAIN_MENU];
    model::init();
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
    view::prepareFrame();
    if(currentScene == &scenes[GAME])
    {
        view::drawWorld(model::getCurrentWorld());
    }
    view::drawScene(*currentScene);
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
    //if click outside current field, deactivate it
    if(currentField && !componentHandler::mouseInside(currentField->getCompID()))
    {
        currentField->deactivate();
    }
    if(e.button.state == SDL_PRESSED)
    {
        Button* btnPtr;
        intRect_t* curRect;
        for(int i = 0; i < int(currentScene->getButtons().size()); i++)
        {
            btnPtr = &(currentScene->getButtons())[i];
            curRect = &componentHandler::getCompIntRect(btnPtr->getCompID());
            if(curRect->x < mouseX && curRect->x + curRect->w > mouseX
               && curRect->y < mouseY && curRect->y + curRect->h > mouseY)
            {
                (*(btnPtr->getCallback())) (btnPtr->getCompID());
                break;
            }
        }
        Field* fieldPtr;
        for(int i = 0; i < int(currentScene->getFields().size()); i++)
        {
            fieldPtr = &(currentScene->getFields())[i];
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
    SDL_GetMouseState(&mouseX, &mouseY);
    intRect_t* tempRect;
    Button* btnPtr;
    for(int i = 0; i < int(currentScene->getButtons().size()); i++)
    {
        tempRect = &componentHandler::getCompIntRect(currentScene->getButtons()[i].getCompID());
        btnPtr = &currentScene->getButtons()[i];
        if(tempRect->x <= mouseX && tempRect->x + tempRect->w > mouseX
           && tempRect->y <= mouseY && tempRect->y + tempRect->h > mouseY)
        {
            btnPtr->setMouseOver(true);
        }
        else
        {
            btnPtr->setMouseOver(false);
        }
    }
    ScrollBlock* sbPtr;
    for(int i = 0; i < int(currentScene->getScrollBlocks().size()); i++)
    {
        sbPtr = &(currentScene->getScrollBlocks())[i];
        tempRect = &componentHandler::getCompIntRect(sbPtr->getCompID());
        if(tempRect->x <= mouseX && tempRect->x + tempRect->w > mouseX
           && tempRect->y <= mouseY && tempRect->y + tempRect->h > mouseY)
        {
            sbPtr->processMouseMotionEvent(e.motion);
        }
    }
}

void Control::processMouseWheelEvent(SDL_Event &e)
{
    ScrollBlock* sbptr;
    intRect_t* rectptr;
    //process all scrollblocks in the screen with the scroll amount
    for(int i = 0; i < int(currentScene->getScrollBlocks().size()); i++)
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
            view::updateWindowSize();
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
    Button startGame(320, 180, 240, 100, "Start Game", &ui::mainStartButton);
    mainMenu.addButton(startGame);
    Button quitGame(320, 300, 240, 100, "Quit Game", &ui::mainQuitButton);
    mainMenu.addButton(quitGame);
    scenes[MAIN_MENU] = mainMenu;
    /* Save menu */
    Scene saveMenu;
    int numSaves = sman->getNumSaves();
    ScrollBlock saveList(320, 180, 550, 300, 550, 90 + 50 * numSaves + BORDER_WIDTH);
    if(numSaves != 0)
    {
        for(int i = 0; i < sman->getNumSaves(); i++)
        {
            Field nameField(250, 50 + 50 * i, 400, 40, sman->listSaves()[i], &ui::saveNameUpdate);
            saveList.addField(nameField);
            Button nameButton(550, 50 + 50 * i, 100, 40, "Load", &ui::loadSave);
            saveList.addButton(nameButton);
        }
    }
    Field newNameField(250, 50 + 50 * numSaves, 400, 40, "", &ui::newSaveNameUpdate);
    saveList.addField(newNameField);
    Button newNameButton(550, 50 + 50 * numSaves, 100, 40, "Create", &ui::newSaveCreate);
    saveList.addButton(newNameButton);
    saveMenu.addScrollBlock(saveList);
    Button backToMain(320, 400, 300, 80, "Back", &ui::saveBackButton);
    saveMenu.addButton(backToMain);
    scenes[SAVE_MENU] = saveMenu;
}

void Control::clearEnables()      //clear currentField and button hovers in current scene
{
    if(currentField != nullptr)
    {   //force same action as pressing "Enter" would have done, whatever that is
        (*currentField->getCallback()) (currentField->getCompID());
        currentField = nullptr;
    }
    for(int i = 0; i < int(currentScene->getButtons().size()); i++)
    {
        currentScene->getButtons()[i].setMouseOver(false);
    }
}
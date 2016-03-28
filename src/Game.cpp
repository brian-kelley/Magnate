#include "Game.h"

using namespace std;

bool Game::terminating = false;

void Game::init()
{
    Renderer::init();
    Coord::initCoord();
    Input::init();
    Camera::init();
    terminating = false;
    Menus::initAll();
    Input::windowBroadcaster.addListener(NULL, processWindow);
    Input::miscBroadcaster.addListener(NULL, processMisc);
    Input::dt = 0;
    Menus::bc.addListener(NULL, processMenuEvent);
    //Load a test world
    debugInit();
    mainLoop();
}

void Game::update()
{
    //Process input (passes to GUI and any other keyboard/mouse listeners)
    Input::update();
    Camera::update();
    //Update world
    World::update();
    Renderer::update();
}

void Game::mainLoop()
{
    //TODO: Have precise software FPS limit framerate without busy waiting
    //For now, run frames as quick as possible
    auto seconds = time(NULL);
    int frameCount = 0;
    while(true)
    {
        auto highPrecStart = chrono::high_resolution_clock::now();
        update();
        if(terminating)
            break;
        auto current = time(NULL);
        frameCount++;
        if(seconds != current)
        {
            //compute fps from time elapsed
            int fps = frameCount / (current - seconds);
            Menus::updateFPS(fps);
            frameCount = 0;
            seconds = current;
        }
        //update Input::dt for the previous frame
        Input::dt = chrono::duration<double>(chrono::high_resolution_clock::now() - highPrecStart).count();
    }
}

void Game::processWindow(void*, const SDL_WindowEvent &event)
{
    if(event.event == SDL_WINDOWEVENT_CLOSE)
        terminating = true;
}

void Game::processMisc(void*, const SDL_EventType &event)
{
    if(event == SDL_QUIT)
        terminating = true;
}

void Game::processMenuEvent(void*, const GeneralMsg& msg)
{
    switch(msg.eventType)
    {
        case MenuEvent::GAME_QUIT:
            terminating = true;
            break;
        default:;
    }
}

void Game::debugInit()
{
    World::initDebug();
    GUI::transition(Menus::gameScene);
}

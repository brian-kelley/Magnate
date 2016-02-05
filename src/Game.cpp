#include "Game.h"

using namespace std;

Game::Game() : renderer()
{
    Coord::initCoord();
    Input::init();
    Camera::init();
    //TODO: Find where to put main menu and initialize here
    terminating = false;
    Menus::initAll();
    Input::windowBroadcaster.addListener(this, processWindow);
    Input::miscBroadcaster.addListener(this, processMisc);
    Input::dt = 0;
    Menus::bc.addListener(this, processMenuEvent);
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
    renderer.update();
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

void Game::processWindow(void *inst, const SDL_WindowEvent &event)
{
    if(event.event == SDL_WINDOWEVENT_CLOSE)
    {
        Game* game = (Game*) inst;
        game->terminating = true;
    }
}

void Game::processMisc(void *inst, const SDL_EventType &event)
{
    if(event == SDL_QUIT)
    {
        Game* game = (Game*) inst;
        game->terminating = true;
    }
}

void Game::processMenuEvent(void* inst, const GeneralMsg& msg)
{
    switch(msg.eventType)
    {
        case MenuEvent::GAME_QUIT:
            ((Game*) inst)->terminating = true;
            break;
        default:;
    }
}

void Game::debugInit()
{
    World::initDebug();
    GUI::transition(Menus::gameScene);
}
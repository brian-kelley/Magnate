#include "Game.h"

using namespace std;
using namespace Coord;

Game::Game() : renderer()
{
    Input::init();
    Camera::init();
    //TODO: Find where to put main menu and initialize here
    terminating = false;
    Menus::initAll();
    Input::windowBroadcaster.addListener(this, processWindow);
    Input::miscBroadcaster.addListener(this, processMisc);
    Menus::bc.addListener(this, processMenuEvent);
    //Load a test world
    World::initDebug();
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
    auto programStart = time(NULL);
    auto seconds = time(NULL);
    int frameCount = 0;
    while(true)
    {
        update();
        if(terminating)
            break;
        auto current = time(NULL);
        frameCount++;
        if(seconds != current)
        {
            //compute fps from time elapsed
            int fps = frameCount / (current - seconds);
            cout << fps << " FPS" << " during second " << current - programStart << endl;
            frameCount = 0;
            seconds = current;
        }
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
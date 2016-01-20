#include "Game.h"

using namespace std;
using namespace Coord;

Game::Game() : renderer()
{
    Input::init();
    Camera::init();
    //TODO: Find where to put main menu and initialize here
    terminating = false;
    initScenes();
    Input::windowBroadcaster.addListener(this, processWindow);
    Input::miscBroadcaster.addListener(this, processMisc);
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

void Game::initScenes()
{
    //Main menu
    new Button(320, 200, 240, 100, StickyDirs::none, "Start Game", Callback(mainStart, this), &mainMenu);
    new Button(320, 320, 240, 100, StickyDirs::none, "Quit Game", Callback(mainQuit, this), &mainMenu);
    new Label(320, 90, 200, 100, StickyDirs::top, "Magnate", &mainMenu);
    //Tell the GUI which scene to start with
    GUI::init(&mainMenu);
}

void Game::mainLoop()
{
    //TODO: Have software limit framerate without busy waiting
    //For now, run frames as quick as possible
    while(true)
    {
        update();
        if(terminating)
            break;
    }
    /*
    while(true)
    {
        bool print = true;
        //Get current (high-precision) time
        auto start = chrono::high_resolution_clock::now();
        update();
        if(terminating)
            break;
        //Busy wait to get exactly 1/60 seconds for total loop
        double time = 0;
        while(time < 1.0 / 60)
        {
            auto end = chrono::high_resolution_clock::now();
            double time = chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000000.0;
            if(print)
            {
                cout << "Frame took " << time << " seconds" << endl;
                print = false;
            }
        }
    }
     */
}

void Game::mainStart(void *inst, void *)
{
    //Transition to save menu
    Game* game = (Game*) inst;
    GUI::transition(&game->saveMenu);
}

void Game::mainQuit(void *inst, void *)
{
    //Signal to quit
    Game* game = (Game*) inst;
    game->terminating = true;
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
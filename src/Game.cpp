#include "Game.h"

using namespace std;
using namespace Coord;

Game::Game() : renderer()
{
    Input::init();
    //TODO: Find where to put main menu and initialize here
    terminating = false;
}

void Game::update()
{
    //Process input (passes to GUI and any other keyboard/mouse listeners)
    Input::update();
    //Update world
    World::update();
    renderer.update();
}

/*
void Game::initScenes()
{
    //Main menu
    Scene* mainMenu = new Scene();
    new Button(320, 200, 240, 100, "Start Game", ui::mainStartButton, mainMenu);
    new Button(320, 320, 240, 100, "Quit Game", ui::mainQuitButton, mainMenu);
    new Label(320, 90, 200, 100, "Magnate", mainMenu);
    scenes[MAIN_MENU] = mainMenu;
    scenes[SAVE_MENU] = SaveManager::getScene();
    Scene* gameOverlay = new Scene();
    scenes[GAME] = gameOverlay;
}
*/

void Game::mainLoop()
{
    while(true)
    {
        //Get current (high-precision) time
        auto start = chrono::high_resolution_clock::now();
        update();
        if(terminating)
            break;
        //Busy wait to get exactly 1/60 seconds for total loop
        while(true)
        {
            auto end = chrono::high_resolution_clock::now();
            auto duration = end - start;
            if(duration.count() >= 1.0 / 60.0)
                break;
        }
    }
}

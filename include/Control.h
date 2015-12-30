#ifndef __MagIndev__Control__
#define __MagIndev__Control__

#ifdef __APPLE__
#include "SDL.h"
#include <boost/filesystem.hpp>
#elif __linux
#include <SDL2/SDL.h>
#elif _WIN32
#include <SDL2/SDL.h>
#endif
#include <cstdint>
#include <iostream>
#include <vector>
#include <chrono>
#include "View.h"
#include "Model.h"
#include "Constants.h"
#include "GenTypes.h"
#include "GUI.h"

enum struct GameState
{
    MainMenu,
    SaveMenu,
    Game
};

class Control
{
public:
    Control();
    ~Control();
private:
    void mainLoop();
    void update();
    GameState state;
    View view;
    Model model;
    SDL_Event* currentEvent;
    bool terminating;        //whether the program will terminate after this frame
    bool updatingView;       //false if window not visible
    bool trackingMouse;      //false if window doesn't have mouse focus
    bool trackingKeyboard;   //false if window doesn't have keyboard focus
    bool camUpdated;         //whether the game camera was moved this frame
    u8* keystate;            //states of all keys
};

/*
    void update();
    bool isTerminating();
    void processWindowResize();
    void processWindowEvent(SDL_Event& e);
    void processMouseMotionEvent();
    void processMouseButtonEvent(SDL_Event& e);
    void processMouseWheelEvent(SDL_Event& e);
    void processKeyEvent(SDL_Event& e);
    void processKeyTypedEvent(SDL_Event& e);
    void updateUISize();
    extern const Uint8* keystate;   //array of key states for all scancodes
 */

#endif
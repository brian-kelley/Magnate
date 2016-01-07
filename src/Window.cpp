#include "Window.h"

using namespace std;

Window::Window(int w, int h)
{
    //Initialize SDL window
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
        cout << "Failed to SDL video." << endl;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    sdlWindow = SDL_CreateWindow("Magnate", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    DBASSERT(sdlWindow);
    glContext = SDL_GL_CreateContext(sdlWindow);
    DBASSERT(glContext);
    SDL_GL_SetSwapInterval(1);
    sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED);
    configGL();
}

SDL_Renderer* Window::getRenderer()
{
    return sdlRenderer;
}

void Window::configGL()
{
    glClearColor(1, 1, 1, 1);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

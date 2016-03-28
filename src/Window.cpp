#include "Window.h"

using namespace std;

int Window::w;
int Window::h;
SDL_Window* Window::sdlWindow;
SDL_Renderer* Window::sdlRenderer;
SDL_GLContext Window::glContext;

void Window::init(int w, int h)
{
    Window::w = w;
    Window::h = h;
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

void Window::dispose()
{
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
    IMG_Quit();
    SDL_Quit();
}

SDL_Renderer* Window::getRenderer()
{
    return sdlRenderer;
}

void Window::configGL()
{
    glClearColor(1, 1, 1, 1);
    glClearDepth(0);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Window::prepareFrame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::endFrame()
{
    //flip framebuffers to update window contents
    SDL_GL_SwapWindow(sdlWindow);
}

Pos2 Window::getSize()
{
    return Pos2(w, h);
}

void Window::resize(int w, int h)
{
    SDL_SetWindowSize(sdlWindow, w, h);
}

void Window::toggleFullscreen(WindowMode wm)
{
    SDL_WindowFlags flags;
    switch(wm)
    {
        case WindowMode::WINDOW:
            flags = (SDL_WindowFlags) 0;
            break;
        case WindowMode::WINDOWED_FULLSCREEN:
            flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
            break;
        case WindowMode::FULLSCREEN:
            flags = SDL_WINDOW_FULLSCREEN;
    }
    SDL_SetWindowFullscreen(sdlWindow, flags);
}

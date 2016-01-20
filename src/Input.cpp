#include "Input.h"

const u8* Input::keystate;
int Input::mouseX;
int Input::mouseY;
Broadcaster<SDL_KeyboardEvent> Input::keyBroadcaster;
Broadcaster<SDL_TextInputEvent> Input::typingBroadcaster;
Broadcaster<SDL_MouseButtonEvent> Input::buttonBroadcaster;
Broadcaster<SDL_MouseMotionEvent> Input::motionBroadcaster;
Broadcaster<SDL_MouseWheelEvent> Input::wheelBroadcaster;
Broadcaster<SDL_WindowEvent> Input::windowBroadcaster;
Broadcaster<SDL_EventType> Input::miscBroadcaster;

void Input::init()
{
    keystate = SDL_GetKeyboardState(NULL);
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

void Input::update()
{
    SDL_PumpEvents();
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                keyBroadcaster.send(event.key);
                break;
            case SDL_TEXTINPUT:
                typingBroadcaster.send(event.text);
                break;
            case SDL_WINDOWEVENT:
                windowBroadcaster.send(event.window);
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                buttonBroadcaster.send(event.button);
                break;
            case SDL_MOUSEMOTION:
                motionBroadcaster.send(event.motion);
                SDL_GetMouseState(&mouseX, &mouseY);
                break;
            case SDL_MOUSEWHEEL:
                wheelBroadcaster.send(event.wheel);
                break;
            default:
                miscBroadcaster.send(SDL_EventType(event.type));
        }
    }
}
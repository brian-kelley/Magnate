#include "Input.h"

const u8* Input::keystate;
const u8* Input::keystateCurrent;
int Input::numKeys = 0;
int Input::mouseX;
int Input::mouseY;
int Input::winX;
int Input::winY;
bool Input::isMouseDown;
float Input::dt;
Broadcaster<SDL_KeyboardEvent> Input::keyBroadcaster;
Broadcaster<SDL_TextInputEvent> Input::typingBroadcaster;
Broadcaster<SDL_MouseButtonEvent> Input::buttonBroadcaster;
Broadcaster<SDL_MouseMotionEvent> Input::motionBroadcaster;
Broadcaster<SDL_MouseWheelEvent> Input::wheelBroadcaster;
Broadcaster<SDL_WindowEvent> Input::windowBroadcaster;
Broadcaster<SDL_EventType> Input::miscBroadcaster;

void Input::init()
{
  keystateCurrent = SDL_GetKeyboardState(&numKeys);
  keystate = new u8[numKeys];
  copyKeystate();
  //SDL_SetRelativeMouseMode(SDL_TRUE);
  winX = GlobalConfig::WINDOW_W;
  winY = GlobalConfig::WINDOW_H;
}

void Input::update()
{
  SDL_PumpEvents();
  SDL_Event event;
  isMouseDown = SDL_GetMouseState(&mouseX, &mouseY) & SDL_BUTTON(SDL_BUTTON_LEFT);
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
        {
          SDL_WindowEvent& wevent = event.window;
          if(wevent.event == SDL_WINDOWEVENT_RESIZED)
          {
            winX = wevent.data1;
            winY = wevent.data2;
          }
          windowBroadcaster.send(wevent);
          break;
        }
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
        buttonBroadcaster.send(event.button);
        break;
      case SDL_MOUSEMOTION:
        motionBroadcaster.send(event.motion);
        break;
      case SDL_MOUSEWHEEL:
        wheelBroadcaster.send(event.wheel);
        break;
      default:
        miscBroadcaster.send(SDL_EventType(event.type));
    }
  }
  copyKeystate();
}

void Input::copyKeystate()
{
  memcpy((void*) keystate, (void*) keystateCurrent, numKeys);
}

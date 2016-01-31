#ifndef __INPUT_H__
#define __INPUT_H__

#include <iostream>
#include "GenTypes.h"
#include "SdlHeaders.h"
#include "Broadcaster.h"
#include "GlobalConfig.h"

namespace Input
{
    void init();
    void update();
    void copyKeystate();
    extern const u8* keystate;  //bool buffer of all keys to read from (access with SDL_SCANCODE_*)
    extern const u8* keystateCurrent;   //bool buffer of all keys that SDL writes to (don't read)
    extern int numKeys;
    extern int mouseX;
    extern int mouseY;
    extern bool isMouseDown;
    extern int winX;
    extern int winY;
    extern float dt;
    extern Broadcaster<SDL_KeyboardEvent> keyBroadcaster;
    extern Broadcaster<SDL_TextInputEvent> typingBroadcaster;
    extern Broadcaster<SDL_MouseButtonEvent> buttonBroadcaster;
    extern Broadcaster<SDL_MouseMotionEvent> motionBroadcaster;
    extern Broadcaster<SDL_MouseWheelEvent> wheelBroadcaster;
    extern Broadcaster<SDL_WindowEvent> windowBroadcaster;
    extern Broadcaster<SDL_EventType> miscBroadcaster;
}

#endif
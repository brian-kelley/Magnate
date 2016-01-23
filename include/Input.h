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
    extern const u8* keystate;  //bool buffer of all keys (access with SDL_SCANCODE_*)
    extern int mouseX;
    extern int mouseY;
    extern int winX;
    extern int winY;
    extern Broadcaster<SDL_KeyboardEvent> keyBroadcaster;
    extern Broadcaster<SDL_TextInputEvent> typingBroadcaster;
    extern Broadcaster<SDL_MouseButtonEvent> buttonBroadcaster;
    extern Broadcaster<SDL_MouseMotionEvent> motionBroadcaster;
    extern Broadcaster<SDL_MouseWheelEvent> wheelBroadcaster;
    extern Broadcaster<SDL_WindowEvent> windowBroadcaster;
    extern Broadcaster<SDL_EventType> miscBroadcaster;
}

#endif
#ifndef __GUI_H__
#define __GUI_H__

#include "Broadcaster.h"
#include "Component.h"
#include "Scene.h"
#include "Button.h"
#include "Label.h"
#include "Field.h"
#include "MultiSelect.h"
#include "ScrollBlock.h"
#include "SdlHeaders.h"
#include "Input.h"

namespace GUI
{
    void init(Scene* initial);
    void transition(Scene* next);
    extern Scene* current;
    //Event listeners
    void processKeyEvent(const SDL_KeyboardEvent& event);
    void processTypingEvent(const SDL_TextInputEvent& event);
    void processButton(const SDL_MouseButtonEvent& event);
    void processMotion(const SDL_MouseMotionEvent& event);
    void processWheel(const SDL_MouseWheelEvent& event);
    void processWindow(const SDL_WindowEvent& event);
}

#endif
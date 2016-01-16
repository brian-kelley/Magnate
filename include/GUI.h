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

class GUI
{
public:
    static void init(Scene* initial);
    static void transition(Scene* next);
    static Scene* current;
private:
    //Event listeners
    static void processKeyEvent(void*, const SDL_KeyboardEvent& event);
    static void processTypingEvent(void*, const SDL_TextInputEvent& event);
    static void processButton(void*, const SDL_MouseButtonEvent& event);
    static void processMotion(void*, const SDL_MouseMotionEvent& event);
    static void processWheel(void*, const SDL_MouseWheelEvent& event);
    static void processWindow(void*, const SDL_WindowEvent& event);
};

#endif
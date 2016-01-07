#include "GUI.h"

Scene* GUI::current;

void GUI::init(Scene *initial)
{
    current = initial;
    //register input listeners
    Input::keyBroadcaster.addListener(processKeyEvent);
    Input::typingBroadcaster.addListener(processTypingEvent);
    Input::buttonBroadcaster.addListener(processButton);
    Input::motionBroadcaster.addListener(processMotion);
    Input::wheelBroadcaster.addListener(processWheel);
    Input::windowBroadcaster.addListener(processWindow);
}

void GUI::transition(Scene *next)
{
    current = next;
}

void GUI::processKeyEvent(const SDL_KeyboardEvent &event)
{
    current->keyEvent(event);
}

void GUI::processTypingEvent(const SDL_TextInputEvent &event)
{
    current->keyTyped(event);
}

void GUI::processButton(const SDL_MouseButtonEvent &event)
{
    current->mouseButton(event);
}

void GUI::processMotion(const SDL_MouseMotionEvent &event)
{
    current->mouseMotion(event);
}

void GUI::processWheel(const SDL_MouseWheelEvent &event)
{
    current->mouseWheel(event);
}

void GUI::processWindow(const SDL_WindowEvent &event)
{
    auto previousRect = current->getLocalRect();
    switch (event.type)
    {
        case SDL_WINDOWEVENT_RESIZED:
        case SDL_WINDOWEVENT_MAXIMIZED:
        case SDL_WINDOWEVENT_SIZE_CHANGED:
        {
            int newWidth = event.data1;
            int newHeight = event.data2;
            if(previousRect.w != newWidth || previousRect.h != newHeight)
            {
                current->setWidth(newWidth);
                current->setHeight(newHeight);
                current->processResize();
            }
            break;
        }
        default:
            break;
    }
}
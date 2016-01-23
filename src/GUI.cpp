#include "GUI.h"

Scene* GUI::current = nullptr;
Scene* GUI::debugScene = nullptr;

void GUI::init(Scene *initial, Scene *debug)
{
    current = initial;
    debugScene = debug;
    //register input listeners
    Input::keyBroadcaster.addListener(NULL, processKeyEvent);
    Input::typingBroadcaster.addListener(NULL, processTypingEvent);
    Input::buttonBroadcaster.addListener(NULL, processButton);
    Input::motionBroadcaster.addListener(NULL, processMotion);
    Input::wheelBroadcaster.addListener(NULL, processWheel);
    Input::windowBroadcaster.addListener(NULL, processWindow);
}

void GUI::transition(Scene *next)
{
    //If previous scene had a Field with keyboard focus, lose it
    if(Field::currentField)
    {
        Field::currentField->loseFocus();
    }
    //If the new scene has a different size than the window, resize scene before showing
    auto& sceneRect = next->getScreenRect();
    if(sceneRect.w != Input::winX || sceneRect.h != Input::winY)
    {
        next->setWidth(Input::winX);
        next->setHeight(Input::winY);
        next->processResize();
    }
    current = next;
}

void GUI::processKeyEvent(void*, const SDL_KeyboardEvent &event)
{
    current->keyEvent(event);
    debugScene->keyEvent(event);
}

void GUI::processTypingEvent(void*, const SDL_TextInputEvent &event)
{
    current->keyTyped(event);
    debugScene->keyTyped(event);
}

void GUI::processButton(void*, const SDL_MouseButtonEvent &event)
{
    current->mouseButton(event);
    debugScene->mouseButton(event);
}

void GUI::processMotion(void*, const SDL_MouseMotionEvent &event)
{
    current->mouseMotion(event);
    debugScene->mouseMotion(event);
}

void GUI::processWheel(void*, const SDL_MouseWheelEvent &event)
{
    current->mouseWheel(event);
    debugScene->mouseWheel(event);
}

void GUI::processWindow(void*, const SDL_WindowEvent &event)
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
                debugScene->setWidth(newWidth);
                debugScene->setHeight(newHeight);
                debugScene->processResize();
            }
            break;
        }
        default:
            break;
    }
}
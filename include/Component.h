#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <iostream>
#include <vector>
#include "Input.h"
#include "GenTypes.h"
#include "SdlHeaders.h"

enum struct CompType
{
    scene,
    button,
    label,
    field,
    scrollBlock,
    multiSelect,
    minimap,
    draggable
};

typedef void (*callback) (void*);   //UI callback (different from Broadcaster listener)

//Describes how component will
namespace StickyDirs
{
    enum : u8
    {
        top = 1 << 0,
        right = 1 << 1,
        bottom = 1 << 2,
        left = 1 << 3,
        fixedWidth = 1 << 4,
        fixedHeight = 1 << 5
    };
}

class Component
{
public:
    Component(int x, int y, int width, int height, u8 stickyFlags, bool center, Component* parentComp);
    virtual ~Component();
    Component* getParent();
    //triggered by window resize
    virtual void updateScreenRect();
    //accessors for local and screen rectangles;
    const Rectangle& getLocalRect();
    const Rectangle& getScreenRect();
    virtual void processResize();       //resize so borders in 4 directions constant
    std::vector<Component*>& getChildren();
    bool isMouseOver();
    //Event processing
    virtual void mouseMotion(const SDL_MouseMotionEvent& event);
    virtual void mouseButton(const SDL_MouseButtonEvent& event);
    virtual void mouseWheel(const SDL_MouseWheelEvent& event);
    virtual void keyTyped(const SDL_TextInputEvent& event);
    virtual void keyEvent(const SDL_KeyboardEvent& event);
    //(For drawing) Get the CompType
    virtual CompType getType() = 0;
protected:
    Pos2 getLocalMouse();
    void addChild(Component* child);    //add a direct subcomponent to my list
    Rectangle local;    //rectangle within parent
    Rectangle canvas;   //space for subcomponents (always 0,0)
    Rectangle screen;   //actual location on screen
    int parentWidth;    //cache parent dimensions (for resize)
    int parentHeight;
    Component* parent;
    std::vector<Component*> children;   //components with this as parent
    u8 stickyFlags;                     //when parent resizes, do dimensions and spaces scale (false) or stay constant (true)
};

#endif
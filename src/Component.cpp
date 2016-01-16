#include "Component.h"

using namespace std;

Component::Component(int x, int y, int width, int height, u8 stickyFlags, bool center, Component* parentComp) : local(x, y, width, height), canvas(x, y, width, height)
{
    this->parent = parentComp;
    this->stickyFlags = stickyFlags;
    updateScreenRect();
}

Component::~Component()
{
    for(int i = 0; i < int(children.size()); i++)
    {
        delete children[i];
    }
    children.clear();
}

bool Component::isMouseOver()
{
    if(screen.x <= Input::mouseX && screen.x + screen.w > Input::mouseX
       && screen.y <= Input::mouseY && screen.y + screen.h > Input::mouseY)
        return true;
    else
        return false;
}

vector<Component*>& Component::getChildren()
{
    return children;
}

void Component::mouseButton(const SDL_MouseButtonEvent &event)
{
    for(Component* c : children)
    {
        if(c->isMouseOver())
            c->mouseButton(event);
    }
}

void Component::mouseMotion(const SDL_MouseMotionEvent &event) {}

void Component::mouseWheel(const SDL_MouseWheelEvent &event)
{
    for(Component* c : children)
    {
        if(c->isMouseOver())
            c->mouseWheel(event);
    }
}

void Component::keyTyped(const SDL_TextInputEvent &event)
{
    for(Component* c : children)
    {
        if(c->isMouseOver())
            c->keyTyped(event);
    }
}

void Component::keyEvent(const SDL_KeyboardEvent& event)
{
    for(Component* c : children)
    {
        if(c->isMouseOver())
            c->keyEvent(event);
    }
}

void Component::addChild(Component* child)
{
    children.push_back(child);
}

Component* Component::getParent()
{
    return parent;
}

void Component::processResize()
{
    //Get the change in parent's dimensions
    float xchg = float(parent->local.w - parentWidth) / parent->local.w;
    parentWidth = parent->local.w;
    float ychg = float(parent->local.h - parentHeight) / parent->local.h;
    parentHeight = parent->local.h;
    //If sticky (true), distance to parent edge stays constant
    //If floating, scale edge to edge distance proportionally in that direction
    int left = local.x;
    int right = parentWidth - local.x - local.w;
    int top = local.y;
    int bottom = parentHeight - local.y - local.h;
    if(!(stickyFlags & StickyDirs::left))
        left *= xchg;
    if(!(stickyFlags & StickyDirs::right))
        right *= xchg;
    if(!(stickyFlags & StickyDirs::top))
        top *= ychg;
    if(!(stickyFlags & StickyDirs::bottom))
        bottom *= ychg;
    //now set new dimensions based on new bounding box
    local.x = left;
    local.y = top;
    int oldW = local.w;
    int oldH = local.h;
    local.w = right - left;
    local.h = bottom - top;
    if(stickyFlags & StickyDirs::fixedWidth)
    {
        //set width back to original, but translate x to maintain position
        int wchg = local.w - oldW;
        local.x += wchg / 2;
        local.w = oldW;
    }
    if(stickyFlags & StickyDirs::fixedHeight)
    {
        int hchg = local.h - oldH;
        local.y += hchg / 2;
        local.w = oldH;
    }
    updateScreenRect();                 //refresh drawing rectangle
    for(auto c : children)              //pass to children
        c->processResize();
}

void Component::updateScreenRect()
{
    //w, h are same local
    screen.w = local.w;
    screen.h = local.h;
    //x, y depend on parent screen position
    //parent needs to have already calculated its own screen position
    screen.x = local.x + parent ? parent->getScreenRect().x : 0;
    screen.y = local.y + parent ? parent->getScreenRect().y : 0;
}

const Rectangle& Component::getLocalRect()
{
    return local;
}

const Rectangle& Component::getScreenRect()
{
    return screen;
}

Pos2 Component::getLocalMouse()
{
    return Pos2(Input::mouseX - screen.x, Input::mouseY - screen.y);
}
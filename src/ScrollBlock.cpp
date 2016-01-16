#include "ScrollBlock.h"

using namespace std;

ScrollBlock::ScrollBlock(int x, int y, int width, int height, u8 stickyFlags, Component* parentComp, int canvh, bool center) : Component(x, y, width, height, stickyFlags, true, parentComp)
{
    canvas.h = canvh;
    viewport = 0;   //start at top of scrollable area
    calcBarPlacement();
}

void ScrollBlock::updateCanvasHeight(int newHeight) //call this to get more space for subcomponents
{
    //minimum height of canvas is height of top-level rect; least height where no scrolling
    if(newHeight < local.h)
        canvas.h = local.h;
    else
        canvas.h = newHeight;
    calcBarPlacement();
    updateScreenRect();
}

void ScrollBlock::processScroll(SDL_MouseWheelEvent& e)
{
    if(this->hasBar())
    {
        int delta = e.y * -4;
        if(delta < 0)	//upward scrolling
        {
            viewport += delta;
            //clamp top of viewport
            if(viewport < 0)
                viewport = 0;
        }
        else if(delta > 0)
        {
            viewport += delta;
            //clamp bottom of viewport
            if(viewport + screen.h > canvas.h)
                viewport = canvas.h - screen.h;
        }
        updateScreenRect();
        calcBarPlacement();
        for(Component* c : children)
        {
            c->updateScreenRect();
        }
    }
}

void ScrollBlock::processMouseMotionEvent(SDL_MouseMotionEvent &e) {}

void ScrollBlock::calcBarPlacement()
{
    if(local.h >= canvas.h)
    {
        barHeight = -1;     //-1 means don't draw any bar
        barPos = -1;
    }
    else
    {
        barHeight = float(local.h) / canvas.h;
        barPos = float(viewport + local.h / 2) / canvas.h - barHeight / 2;
    }
}

bool ScrollBlock::hasBar()
{
    if(barHeight == -1)
        return false;
    else
        return true;
}

int ScrollBlock::getCanvasHeight()
{
    return canvas.h;
}

void ScrollBlock::matchCanvasToContents()
{
    int maxY = 0;
    int bottom;
    for(Component* c : children)
    {
        bottom = c->getLocalRect().y + c->getLocalRect().h;
        if(bottom > maxY)
        {
            maxY = bottom;
        }
    }
    updateCanvasHeight(maxY);
}

void ScrollBlock::updateScreenRect()
{
    Component::updateScreenRect();
    calcBarPlacement();
}

CompType ScrollBlock::getType()
{
    return CompType::scrollBlock;
}
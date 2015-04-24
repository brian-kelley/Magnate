//
//  Component.h
//  Magnate
//
//  Created by Brian Kelley on 12/15/14349.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __Magnate__Component__
#define __Magnate__Component__

#ifdef __APPLE__
#include "SDL.h"
#elif __linux
#include <SDL2/SDL.h>
#elif _WIN32
#include <SDL2/SDL.h>
#endif
#include <iostream>
#include <vector>
#include "Constants.h"

enum CTYPE
{
    BUTTON,
    LABEL,
    FIELD,
    SCROLLBLOCK,
    SCENE,
    MULTISELECT,
    DRAGGABLE
};

class Component
{
public:
    Component(int x, int y, int width, int height, bool center, Component* parentComp, CTYPE type);
    virtual ~Component();
    Component* getParent();
    void calcDrawRect();        //calculate absolute x, y of comp
    virtual void calcOffsets();
    virtual void processResize();       //calculate int sizes based on float & parent
    int getXOffset();           //offsets used i n processing mouse, etc
    int getYOffset();           //scroll panel will override
    void addChild(Component* child);    //add a direct subcomponent to my list
    std::vector<Component*>& getChildren();
    bool isMouseOver();
    intRect_t& getDrawRect();
    intRect_t& getLocalRect();
    virtual void processMouseMotion();
    virtual void processLeftClick();
    virtual void processScroll(SDL_MouseWheelEvent& e);
    virtual void activate();
    virtual void deactivate();
    bool isActive();
    CTYPE getType();
protected:
    intRect_t localRect;        //int rectangle with respect to parent
    intRect_t drawRect;         //store draw coords with offset for speed
    floatRect_t localFloatRect; //store float (0 <= x,y <= 1) coords inside parent
    int xOffset;
    int yOffset;
    Component* parent;
    std::vector<Component*> children;   //components with this as parent
    bool active;
    CTYPE type;
};

#endif
//
//  Component.h
//  Magnate
//
//  Created by Brian Kelley on 12/15/14349.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __Magnate__Component__
#define __Magnate__Component__

#define NO_PARENT -1

#include <stdio.h>
#include <vector>
#include "Constants.h"

namespace componentHandler
{
    struct component_t
    {
        intRect_t irect;
        floatRect_t frect;
        int id;
        int parent;
    };
    extern int numIDs;    //unique numerical id for each UI component in the game
    extern int createComponent(int x, int y, int width, int height, bool center = true);
    extern void updateSize(int id);
    extern bool mouseInside(int id);
    extern std::vector<component_t> componentList;
    extern component_t& getComponent(int id);
    extern intRect_t& getCompIntRect(int id);
    extern floatRect_t& getCompFloatRect(int id);
};

#endif
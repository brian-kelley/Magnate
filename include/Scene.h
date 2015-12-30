//
//  Scene.h
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __SCENE_H__
#define __SCENE_H__

#include "Constants.h"
#include "Component.h"

class Scene : public Component
{
public:
    Scene();
    CompType getType();
    void setWidth(int w);
    void setHeight(int h);
    void processResize();
};

#endif
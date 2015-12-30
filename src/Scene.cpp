//
//  Scene.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Scene.h"

using namespace std;
using namespace constants;

Scene::Scene() : Component(0, 0, WINDOW_W, WINDOW_H, StickyDirs::top | StickyDirs::bottom | StickyDirs::left | StickyDirs::right, false, nullptr) {}

CompType Scene::getType()
{
    return CompType::scene;
}

void Scene::setWidth(int w)
{
    local.w = w;
    updateScreenRect();
}

void Scene::setHeight(int h)
{
    local.h = h;
    updateScreenRect();
}

void Scene::processResize()
{
    //new width/height of window must manually be set
    for(auto c : children)
    {
        c->processResize();
    }
}

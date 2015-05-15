#ifndef MINIMAP_H
#define MINIMAP_H

#include <iostream>

#include "Constants.h"
#include "World.h"
#include "Terrain.h"
#include "OpenGL/GL.h"
#include "RenderRoutines.h"

namespace Minimap
{
    void buildTexture(World& world);
    void render();
}

#endif
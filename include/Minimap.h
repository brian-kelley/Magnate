#ifndef MINIMAP_H
#define MINIMAP_H

#include <iostream>

#include "Constants.h"
#include "World.h"
#include "Terrain.h"
#include "RenderRoutines.h"
#include "Coord.h"

#ifdef __APPLE__
#include "gl.h"
#elif _WIN32
#include "gl.h"
#endif

class World;

namespace Minimap
{
    void buildTexture(World& world);
    void render();
    bool mmIsMouseOver();
    void update();  //set screenX/screenY based on mouseX/mouseY
}

#endif

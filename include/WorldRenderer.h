//
//  WorldRenderer.h
//  Magnate
//
//  Created by Brian Kelley on 3/9/1568.
//
//

#ifndef __Magnate__WorldRenderer__
#define __Magnate__WorldRenderer__

#include <stdio.h>
#include <set>
#include "Terrain.h"
#include "World.h"
#include "Coord.h"
#include "Chunk.h"
#include "RenderRoutines.h"
#include "Constants.h"
#include "Renderer.h"

namespace WorldRenderer
{
    void preload();
    void render(World& world);
    void renderNorth(World& world);
    void renderSouth(World& world);
    void renderWest(World& world);
    void renderEast(World& world);
    void panUp();
    void panDown();
    void panLeft();
    void panRight();
    void rotateLeft(); // N > W > S > E > N
    void rotateRight(); // N > E > S > W > N
}

#endif /* defined(__Magnate__WorldRenderer__) */
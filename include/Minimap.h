#ifndef MINIMAP_H
#define MINIMAP_H

#include <iostream>
#include "Constants.h"
#include "World.h"
#include "Terrain.h"
#include "Renderer.h"
#include "RenderRoutines.h"
#include "WorldRenderer.h"
#include "Coord.h"
#include "GlmHeaders.h"

#ifdef __APPLE__
#include "gl.h"
#elif _WIN32
#include "gl.h"
#endif

namespace Minimap
{
    void buildTexture();
    void render();
    bool mmIsMouseOver();
    void initMM(); //set mm <-> world matrices
    void update();  //set screenX/screenY based on mouseX/mouseY
    Pos2 getPosOnMap(glm::vec2 worldPos);
    void plotPosOnMap(glm::vec2 worldPos); //also call vertex2i
    glm::vec2 mapToWorld(Pos2 mapPos); //get the xz back
    void putMinimapPixel(int x, int y, Uint32* buf, int maxHeight);
    const int MINIMAP_SIZE = 256; //pixels in texture and on screen
    const int MINIMAP_BORDER = 20;//amount of space between edge of window and edge of minimap
    extern glm::mat4 tileToMinimap;
    extern glm::mat4 minimapToTile;
}

#endif
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
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

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
    void update();  //set screenX/screenY based on mouseX/mouseY
    void putMinimapPixel(int x, int y, Uint32* buf);
    const int MINIMAP_SIZE = 256; //pixels in texture and on screen
    const int MINIMAP_BORDER = 15;//width of beveled border around minimap
}

#endif
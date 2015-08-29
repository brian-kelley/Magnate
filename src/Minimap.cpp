#include "Minimap.h"

using namespace std;
using namespace constants;
using namespace Renderer;
using namespace RenderRoutines;
using namespace Coord;
using namespace glm;

mat4 Minimap::tileToMinimap;
mat4 Minimap::minimapToTile;

typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

void Minimap::initMM()
{
    float MM_SCALE = float(MINIMAP_SIZE) / WORLD_SIZE;
    tileToMinimap = scale(tileToMinimap, {MM_SCALE, 1, MM_SCALE});
    //todo: apply desired rotations/reflections here to "tileToMinimap"
    minimapToTile = inverse(tileToMinimap);
}

Color colorFromTerrain(GROUND g)
{
    Color rv;
    switch(g)
    {
        case DESERT:
            rv.r = 0xDA;
            rv.g = 0xDB;
            rv.b = 0x6F;
            return rv;
        case FOREST:
            rv.r = 0x00;
            rv.g = 0x9A;
            rv.b = 0x2F;
            return rv;
        case MOUNTAINS:
            rv.r = 0xA0;
            rv.g = 0xA0;
            rv.b = 0xA0;
            return rv;
        case PLAINS:
            rv.r = 0xA8;
            rv.g = 0xDC;
            rv.b = 0x66;
            return rv;
        case WATER:
            rv.r = 0x00;
            rv.g = 0x6c;
            rv.b = 0xFF;
            return rv;
        default:
            rv.r = 0xFF;
            rv.g = 0xFF;
            rv.b = 0xFF;
            return rv;
    }
}

void Minimap::putMinimapPixel(int x, int y, Uint32* buf)
{
    const int COMPRESSION = WORLD_SIZE / Minimap::MINIMAP_SIZE;
    vec2 worldPos = mapToWorld({short(x), short(y)});
    Pos2 tilePos = {short(worldPos.x / TERRAIN_TILE_SIZE), short(worldPos.y / TERRAIN_TILE_SIZE)};
    PRINT("Tile pos: " << tilePos.x << ", " << tilePos.y)
    //array of occurrences of each terrain type
    int occurences[NUM_TYPES];
    for(int i = 0; i < NUM_TYPES; i++)
    {
        occurences[i] = 0;
    }
    int sumHeights = 0;
    for(int i = tilePos.x; i < tilePos.x + COMPRESSION; i++)
    {
        for(int j = tilePos.y; j < tilePos.y + COMPRESSION; j++)
        {
            occurences[World::getGround(i, j)]++;
            sumHeights += World::getHeight(i, j);
        }
    }
    int avgHeight = sumHeights / (COMPRESSION * COMPRESSION);
    GROUND commonest = WATER;
    for(int i = 0; i < NUM_TYPES; i++)
    {
        if(occurences[i] > occurences[commonest])
        {
            commonest = (GROUND) i;
        }
    }
    Color pxColor = colorFromTerrain(commonest);
    double colorMult = 0.8 + 0.2 * avgHeight / 255.0f;
    int newR = pxColor.r * colorMult;
    if(newR < 0)
        newR = 0;
    if(newR > 255)
        newR = 255;
    pxColor.r = newR;
    int newG = pxColor.g * colorMult;
    if(newG < 0)
        newG = 0;
    if(newG > 255)
        newG = 255;
    pxColor.g = newG;
    int newB = pxColor.b * colorMult;
    if(newB < 0)
        newB = 0;
    if(newB > 255)
        newB = 255;
    pxColor.b = newB;
    //Flip y (z in world space)
    y = MINIMAP_SIZE - y - 1;
#if defined(__APPLE__)
    //BGRA
    buf[x + y * Minimap::MINIMAP_SIZE] = 0xFF << 24 | pxColor.r << 16 | pxColor.g << 8 | pxColor.b;
#elif defined(_WIN32)
    //RGBA32
    buf[x + y * Minimap::MINIMAP_SIZE] = pxColor.r << 24 | pxColor.g << 16 | pxColor.b << 8 | 0xFF;
#endif
}

void Minimap::buildTexture()
{
    Uint32* pixelData = (Uint32*) malloc(MINIMAP_SIZE * MINIMAP_SIZE * sizeof(Uint32));
    for(int i = 0; i < MINIMAP_SIZE; i++)
    {
        for(int j = 0; j < MINIMAP_SIZE; j++)
        {
            putMinimapPixel(i, j, pixelData); //note: reverse y to match world x-z orientation
        }
    }
    int minimapTexID = Atlas::tileFromName("minimap");
    int destX = Atlas::tileX(minimapTexID);
    int destY = Atlas::tileY(minimapTexID);
    int destW = Atlas::tileW(minimapTexID);
    int destH = Atlas::tileH(minimapTexID);
    //This client-side buffer is exactly big enough to hold minimap pixels
#ifdef __APPLE__
    glTexSubImage2D(GL_TEXTURE_2D, 0, destX, destY, destW, destH, GL_BGRA, GL_UNSIGNED_BYTE, pixelData);
#elif _WIN32
    glTexSubImage2D(GL_TEXTURE_2D, 0, destX, destY, destW, destH, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
#endif
    //Free pixel buffer
    free(pixelData);
    //Return to drawing to normal front buffer, visible on screen
}

void Minimap::render()
{
    using namespace Coord;
    int texID = Atlas::tileFromName("minimap");
    //base position of minimap (upper-left corner of minimap image)
    int x = WINDOW_W - MINIMAP_BORDER - MINIMAP_SIZE - BORDER_WIDTH;
    int y = MINIMAP_BORDER;
    const double COMPRESSION = double(WORLD_SIZE) / MINIMAP_SIZE;
    //Calculate screen coordinates of frame.
    int framePts[8];
    FrustumCorners corners = getFrustumCorners();
    //PRINT("Corners in world space:")
    //PRINT("(" << corners.upperLeft.x << "," << corners.upperLeft.y << ")")
    //PRINT("(" << corners.upperRight.x << "," << corners.upperRight.y << ")")
    //PRINT("(" << corners.lowerRight.x << "," << corners.lowerRight.y << ")")
    //PRINT("(" << corners.lowerLeft.x << "," << corners.lowerLeft.y << ")")
    //viewCorners has world coordinates of corners
    framePts[0] = corners.upperLeft.x;
    framePts[1] = corners.upperLeft.y;
    framePts[2] = corners.upperRight.x;
    framePts[3] = corners.upperRight.y;
    framePts[4] = corners.lowerRight.x;
    framePts[5] = corners.lowerRight.y;
    framePts[6] = corners.lowerLeft.x;
    framePts[7] = corners.lowerLeft.y;
    for(int i = 0; i < 8; i++)
    {
        framePts[i] /= TERRAIN_TILE_SIZE;
        framePts[i] /= COMPRESSION;
        if(!(i % 2))
        {
            framePts[i] += x;
            framePts[i + 1] += y;
        }
    }
    bool drawFrame = true;
    for(int i = 0; i < 8; i++)
    {
        if(framePts[i] < 0 || framePts[i] > MINIMAP_SIZE)
        {
            //drawFrame = false;
            break;
        }
    }
    disableTexture();
    color3f(UI_FG_R, UI_FG_G, UI_FG_B);
    /*
    //top beveled edge
    vertex2i(x - BORDER_WIDTH, y - BORDER_WIDTH);
    vertex2i(x + MINIMAP_SIZE + BORDER_WIDTH, y - BORDER_WIDTH);
    vertex2i(x + MINIMAP_SIZE, y);
    vertex2i(x, y);
    //left
    vertex2i(x - BORDER_WIDTH, y - BORDER_WIDTH);
    vertex2i(x, y);
    vertex2i(x, y + MINIMAP_SIZE);
    vertex2i(x - BORDER_WIDTH, y + MINIMAP_SIZE + BORDER_WIDTH);
    //right
    color3f(UI_FG_R * SHADE, UI_FG_G * SHADE, UI_FG_B * SHADE);
    vertex2i(x + MINIMAP_SIZE, y);
    vertex2i(x + MINIMAP_SIZE + BORDER_WIDTH, y - BORDER_WIDTH);
    vertex2i(x + MINIMAP_SIZE + BORDER_WIDTH, y + MINIMAP_SIZE + BORDER_WIDTH);
    vertex2i(x + MINIMAP_SIZE, y + MINIMAP_SIZE);
    //bottom
    vertex2i(x, y + MINIMAP_SIZE);
    vertex2i(x + MINIMAP_SIZE, y + MINIMAP_SIZE);
    vertex2i(x + MINIMAP_SIZE + BORDER_WIDTH, y + MINIMAP_SIZE + BORDER_WIDTH);
    vertex2i(x - BORDER_WIDTH, y + MINIMAP_SIZE + BORDER_WIDTH);
     */
    enableTexture();
    //draw the minimap image
    color3f(1, 1, 1);
    using namespace Atlas;
    texCoord2i(tileX(texID), tileY(texID));
    vertex2i(x + MINIMAP_SIZE, y);
    texCoord2i(tileX(texID) + tileW(texID), tileY(texID));
    vertex2i(x + MINIMAP_SIZE, y + MINIMAP_SIZE);
    texCoord2i(tileX(texID) + tileW(texID), tileY(texID) + tileH(texID));
    vertex2i(x, y + MINIMAP_SIZE);
    texCoord2i(tileX(texID), tileY(texID) + tileH(texID));
    vertex2i(x, y);
    //draw the frame
    if(drawFrame)
    {
        color3f(1, 1, 1);
        Pos2 temp = getPosOnMap(corners.upperLeft);
        lineVertex2i(temp.x, temp.y);
        temp = getPosOnMap(corners.upperRight);
        lineVertex2i(temp.x, temp.y);
        lineVertex2i(temp.x, temp.y);
        temp = getPosOnMap(corners.lowerRight);
        lineVertex2i(temp.x, temp.y);
        lineVertex2i(temp.x, temp.y);
        temp = getPosOnMap(corners.lowerLeft);
        lineVertex2i(temp.x, temp.y);
        lineVertex2i(temp.x, temp.y);
        temp = getPosOnMap(corners.upperLeft);
        lineVertex2i(temp.x, temp.y);
    }
}

void Minimap::update() //this means mouse is over minimap and camera needs to move to corresponding location
{
    Pos2 mousePos = {short(mouseX), short(mouseY)};
    vec2 newCenter = mapToWorld(mousePos);
    camPos = {newCenter.x, camPos.y, newCenter.y};
}

bool Minimap::mmIsMouseOver()
{
    if(mouseX >= WINDOW_W - MINIMAP_BORDER - MINIMAP_SIZE && mouseX < WINDOW_W - MINIMAP_BORDER && mouseY >= MINIMAP_BORDER && mouseY < MINIMAP_BORDER + MINIMAP_SIZE)
        return true;
    return false;
}

Pos2 Minimap::getPosOnMap(glm::vec2 worldPos)
{
    //first apply simple conversion factor to x and y
    vec4 world = {worldPos.x, 0, worldPos.y, 1};
    vec4 map = tileToMinimap * (worldToTileMat * world);
    //now translate tilePos to be in the minimap square onscreen
    Pos2 tilePos = {short(map.x), short(map.z)};
    tilePos.x += (WINDOW_W - MINIMAP_BORDER - MINIMAP_SIZE);
    tilePos.y += MINIMAP_BORDER;
    return tilePos;
}

void Minimap::plotPosOnMap(glm::vec2 worldPos)
{
    Pos2 pos = getPosOnMap(worldPos);
    vertex2i(pos.x, pos.y);
}

vec2 Minimap::mapToWorld(Pos2 mapPos)
{
    vec4 mapVec = {mapPos.x, 0, mapPos.y, 1};
    vec4 world = tileToWorldMat * (minimapToTile * mapVec);
    return {world.x, world.z};
}
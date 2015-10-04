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
        case RIVER:
        case LAKE:
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

void Minimap::putMinimapPixel(int x, int y, Uint32* buf, int maxHeight)
{
    const int COMPRESSION = float(WORLD_SIZE) / Minimap::MINIMAP_SIZE;
    vec2 worldXZ = mapToWorld({short(x), short(y)});
    vec4 worldPos = {worldXZ.x, 0, worldXZ.y, 1};
    Pos2 tilePos = worldToTile(worldPos);
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
            int h = World::getHeight(i, j);
            occurences[World::getGround(i, j)]++;
            sumHeights += h;
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
    if(occurences[RIVER] > 0)
        commonest = RIVER;
    else if(occurences[LAKE] > 0)
        commonest = LAKE;
    Color pxColor = colorFromTerrain(commonest);
    double heightComp = avgHeight / double(maxHeight);
    if(heightComp < 0)
        heightComp = 0;
    double colorMult = 0.8 + 0.6 * heightComp;
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
    Uint32* pixelData = new Uint32[MINIMAP_SIZE * MINIMAP_SIZE];
    int maxHeight = 0;
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            int h = World::getHeight(i, j);
            if(h > maxHeight)
                maxHeight = h;
        }
    }
    for(int i = 0; i < MINIMAP_SIZE; i++)
    {
        for(int j = 0; j < MINIMAP_SIZE; j++)
        {
            putMinimapPixel(i, j, pixelData, maxHeight); //note: reverse y to match world x-z orientation
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
    delete[] pixelData;
    //Return to drawing to normal front buffer, visible on screen
}

void Minimap::render()
{
    using namespace Coord;
    int texID = Atlas::tileFromName("minimap");
    //base position of minimap (upper-left corner of minimap image)
    int x = WINDOW_W - MINIMAP_BORDER - MINIMAP_SIZE - BORDER_WIDTH;
    int y = MINIMAP_BORDER + BORDER_WIDTH;
    //Calculate screen coordinates of frame.
    FrustumCorners corners = getFrustumCorners();
    //PRINT("Corners in world space:")
    //PRINT("(" << corners.upperLeft.x << "," << corners.upperLeft.y << ")")
    //PRINT("(" << corners.upperRight.x << "," << corners.upperRight.y << ")")
    //PRINT("(" << corners.lowerRight.x << "," << corners.lowerRight.y << ")")
    //PRINT("(" << corners.lowerLeft.x << "," << corners.lowerLeft.y << ")")
    //viewCorners has world coordinates of corners
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
    vertex2i(x, y);
    texCoord2i(tileX(texID) + tileW(texID), tileY(texID));
    vertex2i(x + MINIMAP_SIZE, y);
    texCoord2i(tileX(texID) + tileW(texID), tileY(texID) + tileH(texID));
    vertex2i(x + MINIMAP_SIZE, y + MINIMAP_SIZE);
    texCoord2i(tileX(texID), tileY(texID) + tileH(texID));
    vertex2i(x, y + MINIMAP_SIZE);
    //draw the frame
    if(1)
    {
        color3f(1, 1, 1);
        mat4 worldToMap = tileToMinimap * worldToTileMat;
        vec4 points[4];
        points[0] = worldToMap * corners.upperLeft;
        points[1] = worldToMap * corners.upperRight;
        points[2] = worldToMap * corners.lowerRight;
        points[3] = worldToMap * corners.lowerLeft;
        for(int i = 0; i < 4; i++)
        {
            points[i].x += x;
            points[i].z += y;
        }
        lineVertex2i(points[0].x, points[0].z);
        lineVertex2i(points[1].x, points[1].z);
        lineVertex2i(points[1].x, points[1].z);
        lineVertex2i(points[2].x, points[2].z);
        lineVertex2i(points[2].x, points[2].z);
        lineVertex2i(points[3].x, points[3].z);
        lineVertex2i(points[3].x, points[3].z);
        lineVertex2i(points[0].x, points[0].z);
    }
}

void Minimap::update() //this means mouse is over minimap and camera needs to move to corresponding location
{
    //translate mouse position so that upper-left corner of MM is (0,0)
    int localX = mouseX - (WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER - BORDER_WIDTH);
    int localY = mouseY - MINIMAP_BORDER - BORDER_WIDTH;
    Pos2 localMouse = {short(localX), short(localY)};
    vec2 newCenter = mapToWorld(localMouse);
    Camera::camPos = {newCenter.x, Camera::camPos.y, newCenter.y};
    Renderer::update3DMatrices();
    WorldRenderer::updateVBOChunks();
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
    vec4 tileVec = minimapToTile * mapVec;
    vec4 world = tileToWorldMat * tileVec;
    return {world.x, world.z};
}
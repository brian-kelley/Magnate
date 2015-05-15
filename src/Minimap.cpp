#include "Minimap.h"

using namespace std;
using namespace constants;
using namespace Renderer;
using namespace RenderRoutines;

typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

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

void putMinimapPixel(World& world, int x, int y, Uint8* buf)
{
    const double COMPRESSION = floor(double(WORLD_SIZE) / MINIMAP_SIZE);
    //array of occurrences of each terrain type
    int occurences[GROUND::NUM_TYPES];
    for(int i = 0; i < GROUND::NUM_TYPES; i++)
    {
        occurences[i] = 0;
    }
    for(int i = x * COMPRESSION - COMPRESSION / 2; i < x * COMPRESSION + COMPRESSION / 2; i++)
    {
        for(int j = y * COMPRESSION - COMPRESSION / 2; j < y * COMPRESSION + COMPRESSION / 2; j++)
        {
            occurences[world.getHeight(i, j)]++;
        }
    }
    GROUND commonest = WATER;
    for(int i = 0; i < NUM_TYPES; i++)
    {
        if(occurences[i] > occurences[commonest])
        {
            commonest = (GROUND) i;
        }
    }
    Color pxColor = colorFromTerrain(commonest);
    Uint8* pixelPtr = buf + ((MINIMAP_SIZE - y) * MINIMAP_SIZE * 4 + x * 4);
#ifdef __APPLE__
    //Use BGRA as internal 32-bit pixel format
    pixelPtr[0] = 0xFF;
    pixelPtr[1] = 0xFF;
    pixelPtr[2] = 0xFF;
    /*
    pixelPtr[0] = pxColor.b;
    pixelPtr[1] = pxColor.g;
    pixelPtr[2] = pxColor.r;
     */
    pixelPtr[3] = 0xFF;
#elif _WIN32
    //RGBA32
    pixelPtr[0] = pxColor.r;
    pixelPtr[1]  = pxColor.g;
    pixelPtr[2] = pxColor.b;
    pixelPtr[3] = 0xFF;
#endif
}

void Minimap::buildTexture(World& world)
{
    Uint8* pixelData = (Uint8*) malloc(MINIMAP_SIZE * MINIMAP_SIZE * BIT_DEPTH / 8);
    for(int i = 0; i < MINIMAP_SIZE; i++)
    {
        for(int j = 0; j < MINIMAP_SIZE; j++)
        {
            putMinimapPixel(world, i, j, pixelData);
        }
    }
    int minimapTexID = mainAtlas->tileFromName("minimap");
    int destX = mainAtlas->getSize() * mainAtlas->tileX(minimapTexID);
    int destY = mainAtlas->getSize() * mainAtlas->tileY(minimapTexID);
    int destW = mainAtlas->getSize() * mainAtlas->tileW(minimapTexID);
    int destH = mainAtlas->getSize() * mainAtlas->tileH(minimapTexID);
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
    int texID = mainAtlas->tileFromName("minimap");
    color3f(1, 1, 1);
    texCoord2f(mainAtlas->tileX(texID), mainAtlas->tileY(texID));
    vertex2i(WINDOW_W - MINIMAP_SIZE - 15, 15);
    texCoord2f(mainAtlas->tileX(texID) + mainAtlas->tileW(texID), mainAtlas->tileY(texID));
    vertex2i(WINDOW_W - 15, 15);
    texCoord2f(mainAtlas->tileX(texID) + mainAtlas->tileW(texID), mainAtlas->tileY(texID) + mainAtlas->tileH(texID));
    vertex2i(WINDOW_W - 15, 15 + MINIMAP_SIZE);
    texCoord2f(mainAtlas->tileX(texID), mainAtlas->tileY(texID) + mainAtlas->tileH(texID));
    vertex2i(WINDOW_W - MINIMAP_SIZE - 15, 15 + MINIMAP_SIZE);
}
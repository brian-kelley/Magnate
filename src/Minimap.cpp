#include "Minimap.h"

using namespace std;
using namespace constants;
using namespace Renderer;
using namespace RenderRoutines;
using namespace Coord;
using namespace glm;

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

void Minimap::putMinimapPixel(int x, int y, Uint32* buf)
{
    const double COMPRESSION = floor(double(WORLD_SIZE) / Minimap::MINIMAP_SIZE);
    //array of occurrences of each terrain type
    int occurences[NUM_TYPES];
    for(int i = 0; i < NUM_TYPES; i++)
    {
        occurences[i] = 0;
    }
    int sumHeights = 0;
    for(int i = x * COMPRESSION - COMPRESSION / 2; i < x * COMPRESSION + COMPRESSION / 2; i++)
    {
        for(int j = y * COMPRESSION - COMPRESSION / 2; j < y * COMPRESSION + COMPRESSION / 2; j++)
        {
            occurences[World::getGround(i, j)]++;
            sumHeights += World::getHeight(i, j);
        }
    }
    int avgHeight = sumHeights / COMPRESSION / COMPRESSION;
    GROUND commonest = WATER;
    for(int i = 0; i < NUM_TYPES; i++)
    {
        if(occurences[i] > occurences[commonest])
        {
            commonest = (GROUND) i;
        }
    }
    Color pxColor = colorFromTerrain(commonest);
    double colorMult = 0.8 + 0.2 * avgHeight / 255;
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
    int y = BORDER_WIDTH;
    const double COMPRESSION = double(WORLD_SIZE) / MINIMAP_SIZE;
    //Calculate screen coordinates of frame.
    double viewCorners[8];
    int framePts[8];
    Renderer::getFrustumCorners(viewCorners);
    //viewCorners has world coordinates of corners
    for(int i = 0; i < 8; i++)
    {
        viewCorners[i] /= TERRAIN_TILE_SIZE;
        viewCorners[i] /= COMPRESSION;
        if(i % 2)
            //flip y to match orientation of minimap image
            viewCorners[i] = MINIMAP_SIZE - viewCorners[i];
        framePts[i] = viewCorners[i];
    }
    bool drawFrame = true;
    for(int i = 0; i < 8; i++)
    {
        if(framePts[i] < 0 || framePts[i] > MINIMAP_SIZE)
        {
            drawFrame = false;
            break;
        }
    }
    //cout << x1 << "," << y1 << " : " << x2 << "," << y2 << " : " << x3 << "," << y3 << " : " << x4 << "," << y4 << endl;
    disableTexture();
    color3f(UI_FG_R, UI_FG_G, UI_FG_B);
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
}

void Minimap::update() //this means mouse is over minimap and camera needs to move to corresponding location
{
    const double COMPRESSION = double(WORLD_SIZE) * TERRAIN_TILE_SIZE / MINIMAP_SIZE; //world distance per minimap pixel
    double centerX = (mouseX - (WINDOW_W - MINIMAP_BORDER - MINIMAP_SIZE)) * COMPRESSION;
    double centerZ = ((MINIMAP_BORDER + MINIMAP_SIZE) - mouseY) * COMPRESSION;
    double heightMult = camPos.y / camDir.y;
    camPos.x = centerX - camDir.x * heightMult;
    camPos.z = centerZ - camDir.z * heightMult;
}

bool Minimap::mmIsMouseOver()
{
    if(mouseX >= WINDOW_W - MINIMAP_BORDER - MINIMAP_SIZE && mouseX < WINDOW_W - MINIMAP_BORDER && mouseY >= MINIMAP_BORDER && mouseY < MINIMAP_BORDER + MINIMAP_SIZE)
        return true;
    return false;
}
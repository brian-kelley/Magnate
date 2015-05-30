#include "Minimap.h"

using namespace std;
using namespace constants;
using namespace Renderer;
using namespace RenderRoutines;
using namespace coord;

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

void putMinimapPixel(World& world, int x, int y, Uint32* buf)
{
    const double COMPRESSION = floor(double(WORLD_SIZE) / MINIMAP_SIZE);
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
            occurences[world.getGround(i, j)]++;
            sumHeights += world.getHeight(i, j);
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
#if defined(__APPLE__)
    //BGRA
    buf[x + y * MINIMAP_SIZE] = 0xFF << 24 | pxColor.r << 16 | pxColor.g << 8 | pxColor.b;
#elif defined(_WIN32)
    //RGBA32
    buf[x + y * MINIMAP_SIZE] = pxColor.r << 24 | pxColor.g << 16 | pxColor.b << 8 | 0xFF;
#endif
}

void Minimap::buildTexture(World& world)
{
    Uint32* pixelData = (Uint32*) malloc(MINIMAP_SIZE * MINIMAP_SIZE * sizeof(Uint32));
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
    cout << "Building minimap texture." << endl;
#ifdef __APPLE__
    glTexSubImage2D(GL_TEXTURE_2D, 0, destX, destY, destW, destH, GL_BGRA, GL_UNSIGNED_BYTE, pixelData);
#elif _WIN32
    glTexSubImage2D(GL_TEXTURE_2D, 0, destX, destY, destW, destH, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
#endif
    //Free pixel buffer
    free(pixelData);
    //Return to drawing to normal front buffer, visible on screen
}

//Rotate the point clockwise, given that (x, y) is in local minimap coords (0 -> MINIMAP_SIZE)
void rotateCW(int& x, int& y)
{
    int temp = x;
    x = MINIMAP_SIZE - y;
    y = temp;
}
//Counterclockwise
void rotateCCW(int& x, int& y)
{
    int temp = x;
    x = y;
    y = MINIMAP_SIZE - temp;
}

void Minimap::render()
{
    using namespace coord;
    int texID = mainAtlas->tileFromName("minimap");
    color3f(1, 1, 1);
    enableTexture();
    const double COMPRESSION = double(WORLD_SIZE) / MINIMAP_SIZE;
    //Calculate screen coordinates of frame.
    int x1 = xi(screenX, screenY) / COMPRESSION / TERRAIN_TILE_SIZE;
    int y1 = MINIMAP_SIZE - yj(screenX, screenY) / COMPRESSION / TERRAIN_TILE_SIZE;
    int x2 = xi(screenX + WINDOW_W, screenY) / COMPRESSION / TERRAIN_TILE_SIZE;
    int y2 = MINIMAP_SIZE - yj(screenX + WINDOW_W, screenY) / COMPRESSION / TERRAIN_TILE_SIZE;
    int x3 = xi(screenX + WINDOW_W, screenY + WINDOW_H) / COMPRESSION / TERRAIN_TILE_SIZE;
    int y3 = MINIMAP_SIZE - yj(screenX + WINDOW_W, screenY + WINDOW_H) / COMPRESSION / TERRAIN_TILE_SIZE;
    int x4 = xi(screenX, screenY + WINDOW_H) / COMPRESSION / TERRAIN_TILE_SIZE;
    int y4 = MINIMAP_SIZE - yj(screenX, screenY + WINDOW_H) / COMPRESSION / TERRAIN_TILE_SIZE;
    switch(viewDirection)
    {
        case NORTH:
            break;
        case WEST:
            rotateCW(x1, y1);
            rotateCW(x2, y2);
            rotateCW(x3, y3);
            rotateCW(x4, y4);
            break;
        case EAST:
            rotateCCW(x1, y1);
            rotateCCW(x2, y2);
            rotateCCW(x3, y3);
            rotateCCW(x4, y4);
            break;
        case SOUTH:
            x1 = MINIMAP_SIZE - x1;
            y1 = MINIMAP_SIZE - y1;
            x2 = MINIMAP_SIZE - x2;
            y2 = MINIMAP_SIZE - y2;
            x3 = MINIMAP_SIZE - x3;
            y3 = MINIMAP_SIZE - y3;
            x4 = MINIMAP_SIZE - x4;
            y4 = MINIMAP_SIZE - y4;
            break;
    }
    bool drawFrame = true;
    if(x1 < 0 || x2 < 0 || x3 < 0 || x4 < 0 || x1 > MINIMAP_SIZE || x2 > MINIMAP_SIZE || x3 > MINIMAP_SIZE || x4 > MINIMAP_SIZE || y1 < 0 || y2 < 0 || y3 < 0 || y4 < 0 || y1 > MINIMAP_SIZE || y2 > MINIMAP_SIZE || y3 > MINIMAP_SIZE || y4 > MINIMAP_SIZE)
    {
        drawFrame = false;
    }
    //cout << x1 << "," << y1 << " : " << x2 << "," << y2 << " : " << x3 << "," << y3 << " : " << x4 << "," << y4 << endl;
    disableTexture();
    color3f(UI_FG_R, UI_FG_G, UI_FG_B);
    //top beveled edge
    vertex2i(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER - BORDER_WIDTH, MINIMAP_BORDER - BORDER_WIDTH);
    vertex2i(WINDOW_W - MINIMAP_BORDER + BORDER_WIDTH, MINIMAP_BORDER - BORDER_WIDTH);
    vertex2i(WINDOW_W - MINIMAP_BORDER, MINIMAP_BORDER);
    vertex2i(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER, MINIMAP_BORDER);
    //left
    vertex2i(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER - BORDER_WIDTH, MINIMAP_BORDER - BORDER_WIDTH);
    vertex2i(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER, MINIMAP_BORDER);
    vertex2i(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER, MINIMAP_BORDER + MINIMAP_SIZE);
    vertex2i(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER - BORDER_WIDTH, MINIMAP_BORDER + MINIMAP_SIZE + BORDER_WIDTH);
    //right
    color3f(UI_FG_R * SHADE, UI_FG_G * SHADE, UI_FG_B * SHADE);
    vertex2i(WINDOW_W - MINIMAP_BORDER, MINIMAP_BORDER);
    vertex2i(WINDOW_W - MINIMAP_BORDER + BORDER_WIDTH, MINIMAP_BORDER - BORDER_WIDTH);
    vertex2i(WINDOW_W - MINIMAP_BORDER + BORDER_WIDTH, MINIMAP_BORDER + MINIMAP_SIZE + BORDER_WIDTH);
    vertex2i(WINDOW_W - MINIMAP_BORDER, MINIMAP_BORDER + MINIMAP_SIZE);
    //bottom
    vertex2i(WINDOW_W - MINIMAP_BORDER - MINIMAP_SIZE, MINIMAP_BORDER + MINIMAP_SIZE);
    vertex2i(WINDOW_W - MINIMAP_BORDER, MINIMAP_BORDER + MINIMAP_SIZE);
    vertex2i(WINDOW_W - MINIMAP_BORDER + BORDER_WIDTH, MINIMAP_BORDER + MINIMAP_SIZE + BORDER_WIDTH);
    vertex2i(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER - BORDER_WIDTH, MINIMAP_BORDER + MINIMAP_SIZE + BORDER_WIDTH);
    enableTexture();
    color3f(1, 1, 1);
    switch(viewDirection)
    {
        case NORTH:
        {
            drawString("N", 0, 0);
            color3f(1, 1, 1);
            texCoord2f(mainAtlas->tileX(texID), mainAtlas->tileY(texID) + mainAtlas->tileH(texID));
            vertex2i(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER, MINIMAP_BORDER);
            texCoord2f(mainAtlas->tileX(texID) + mainAtlas->tileW(texID), mainAtlas->tileY(texID) + mainAtlas->tileH(texID));
            vertex2i(WINDOW_W - MINIMAP_BORDER, MINIMAP_BORDER);
            texCoord2f(mainAtlas->tileX(texID) + mainAtlas->tileW(texID), mainAtlas->tileY(texID));
            vertex2i(WINDOW_W - MINIMAP_BORDER, MINIMAP_BORDER + MINIMAP_SIZE);
            texCoord2f(mainAtlas->tileX(texID), mainAtlas->tileY(texID));
            vertex2i(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER, MINIMAP_BORDER + MINIMAP_SIZE);
            //Draw frame representing viewport
            if(drawFrame)
            {
                color3f(1, 1, 1);
                disableTexture();
                drawLine(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x1, MINIMAP_BORDER + y1, WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x2, MINIMAP_BORDER + y2);
                drawLine(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x2, MINIMAP_BORDER + y2, WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x3, MINIMAP_BORDER + y3);
                drawLine(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x3, MINIMAP_BORDER + y3, WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x4, MINIMAP_BORDER + y4);
                drawLine(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x1, MINIMAP_BORDER + y1, WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x4, MINIMAP_BORDER + y4);
            }
            break;
        }
        case WEST:
        {
            drawString("W", 0, 0);
            color3f(1, 1, 1);
            texCoord2f(mainAtlas->tileX(texID), mainAtlas->tileY(texID));
            vertex2i(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER, MINIMAP_BORDER);
            texCoord2f(mainAtlas->tileX(texID), mainAtlas->tileY(texID) + mainAtlas->tileH(texID));
            vertex2i(WINDOW_W - MINIMAP_BORDER, MINIMAP_BORDER);
            texCoord2f(mainAtlas->tileX(texID) + mainAtlas->tileW(texID), mainAtlas->tileY(texID) + mainAtlas->tileH(texID));
            vertex2i(WINDOW_W - MINIMAP_BORDER, MINIMAP_BORDER + MINIMAP_SIZE);
            texCoord2f(mainAtlas->tileX(texID) + mainAtlas->tileW(texID), mainAtlas->tileY(texID));
            vertex2i(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER, MINIMAP_BORDER + MINIMAP_SIZE);
            if(drawFrame)
            {
                color3f(1, 1, 1);
                drawLine(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x1, MINIMAP_BORDER + y1, WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x2, MINIMAP_BORDER + y2);
                drawLine(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x2, MINIMAP_BORDER + y2, WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x3, MINIMAP_BORDER + y3);
                drawLine(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x3, MINIMAP_BORDER + y3, WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x4, MINIMAP_BORDER + y4);
                drawLine(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x1, MINIMAP_BORDER + y1, WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x4, MINIMAP_BORDER + y4);
            }
            break;
        }
        case EAST:
        {
            drawString("E", 0, 0);
            color3f(1, 1, 1);
            texCoord2f(mainAtlas->tileX(texID) + mainAtlas->tileW(texID), mainAtlas->tileY(texID) + mainAtlas->tileH(texID));
            vertex2i(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER, MINIMAP_BORDER);
            texCoord2f(mainAtlas->tileX(texID) + mainAtlas->tileW(texID), mainAtlas->tileY(texID));
            vertex2i(WINDOW_W - MINIMAP_BORDER, MINIMAP_BORDER);
            texCoord2f(mainAtlas->tileX(texID), mainAtlas->tileY(texID));
            vertex2i(WINDOW_W - MINIMAP_BORDER, MINIMAP_BORDER + MINIMAP_SIZE);
            texCoord2f(mainAtlas->tileX(texID), mainAtlas->tileY(texID) + mainAtlas->tileH(texID));
            vertex2i(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER, MINIMAP_SIZE + MINIMAP_BORDER);
            if(drawFrame)
            {
                color3f(1, 1, 1);
                drawLine(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x1, MINIMAP_BORDER + y1, WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x2, MINIMAP_BORDER + y2);
                drawLine(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x2, MINIMAP_BORDER + y2, WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x3, MINIMAP_BORDER + y3);
                drawLine(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x3, MINIMAP_BORDER + y3, WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x4, MINIMAP_BORDER + y4);
                drawLine(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x1, MINIMAP_BORDER + y1, WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x4, MINIMAP_BORDER + y4);
            }
            break;
        }
        case SOUTH:
        {
            drawString("S", 0, 0);
            color3f(1, 1, 1);
            texCoord2f(mainAtlas->tileX(texID) + mainAtlas->tileW(texID), mainAtlas->tileY(texID));
            vertex2i(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER, MINIMAP_BORDER);
            texCoord2f(mainAtlas->tileX(texID), mainAtlas->tileY(texID));
            vertex2i(WINDOW_W - MINIMAP_BORDER, MINIMAP_BORDER);
            texCoord2f(mainAtlas->tileX(texID), mainAtlas->tileY(texID) + mainAtlas->tileH(texID));
            vertex2i(WINDOW_W - MINIMAP_BORDER, MINIMAP_BORDER + MINIMAP_SIZE);
            texCoord2f(mainAtlas->tileX(texID) + mainAtlas->tileW(texID), mainAtlas->tileY(texID) + mainAtlas->tileH(texID));
            vertex2i(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER, MINIMAP_BORDER + MINIMAP_SIZE);
            if(drawFrame)
            {
                color3f(1, 1, 1);
                drawLine(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x1, MINIMAP_BORDER + y1, WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x2, MINIMAP_BORDER + y2);
                drawLine(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x2, MINIMAP_BORDER + y2, WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x3, MINIMAP_BORDER + y3);
                drawLine(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x3, MINIMAP_BORDER + y3, WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x4, MINIMAP_BORDER + y4);
                drawLine(WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x1, MINIMAP_BORDER + y1, WINDOW_W - MINIMAP_SIZE - MINIMAP_BORDER + x4, MINIMAP_BORDER + y4);
            }
        }
    }
}

void Minimap::update()
{
    int minimapI = mouseX - (WINDOW_W - MINIMAP_BORDER - MINIMAP_SIZE);
    int minimapJ = MINIMAP_SIZE - mouseY + MINIMAP_BORDER;
    //Factor to multiply minimap pixels by to get equivalent isometric world coords
    const double TILE_COMPRESSION = double(WORLD_SIZE) * TERRAIN_TILE_SIZE / MINIMAP_SIZE;
    switch(viewDirection)
    {
        case NORTH:
        {
            //New location of center of screen
            int newSX = -WINDOW_W / 2 + ix(minimapI * TILE_COMPRESSION, minimapJ * TILE_COMPRESSION);
            int newSY = -WINDOW_H / 2 + jy(minimapI * TILE_COMPRESSION, minimapJ * TILE_COMPRESSION);
            screenX = newSX;
            screenY = newSY;
            break;
        }
        case WEST:
        {
            rotateCW(minimapI, minimapJ);
            int newSX = -WINDOW_W / 2 + ix(minimapI * TILE_COMPRESSION, minimapJ * TILE_COMPRESSION);
            int newSY = -WINDOW_H / 2 + jy(minimapI * TILE_COMPRESSION, minimapJ * TILE_COMPRESSION);
            screenX = newSX;
            screenY = newSY;
            break;
        }
        case EAST:
        {
            rotateCCW(minimapI, minimapJ);
            //New location of center of screen
            int newSX = -WINDOW_W / 2 + ix(minimapI * TILE_COMPRESSION, minimapJ * TILE_COMPRESSION);
            int newSY = -WINDOW_H / 2 + jy(minimapI * TILE_COMPRESSION, minimapJ * TILE_COMPRESSION);
            screenX = newSX;
            screenY = newSY;
            break;
        }
        case SOUTH:
        {
            minimapI = MINIMAP_SIZE - minimapI;
            minimapJ = MINIMAP_SIZE - minimapJ;
            //New location of center of screen
            int newSX = -WINDOW_W / 2 + ix(minimapI * TILE_COMPRESSION, minimapJ * TILE_COMPRESSION);
            int newSY = -WINDOW_H / 2 + jy(minimapI * TILE_COMPRESSION, minimapJ * TILE_COMPRESSION);
            screenX = newSX;
            screenY = newSY;
            break;
        }
    }
}

bool Minimap::mmIsMouseOver()
{
    if(mouseX >= WINDOW_W - MINIMAP_BORDER - MINIMAP_SIZE && mouseX < WINDOW_W - MINIMAP_BORDER && mouseY >= MINIMAP_BORDER && mouseY < MINIMAP_BORDER + MINIMAP_SIZE)
        return true;
    return false;
}
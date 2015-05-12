//
//  WorldRenderer.cpp
//  Magnate
//
//  Created by Brian Kelley on 3/9/1568.
//
//

#include "WorldRenderer.h"

using namespace std;
using namespace constants;
using namespace coord;
using namespace Renderer;

//4 vertices, each with 2 floats (u, v)
float terrainUV[GROUND::NUM_TYPES * 4 * 2];

bool ptInScreen(Point p);
bool chunkInWorld(int x, int y);


void WorldRenderer::preload()
{
    //Init fast array of float rect stuff
    //Ideally this will iterate through every ground type (hopefully)
    int i = 0;
    for(GROUND g = (GROUND) 0; g < NUM_TYPES; g = (GROUND) ((int) g + 1))
    {
        int terrainTexID = Terrain::terrainTextures[g];
        floatRect_t uvrect = RenderRoutines::getTexCoords(terrainTexID);
        float eps = 1e-4;
        terrainUV[8 * i + 0] = uvrect.x + eps;
        terrainUV[8 * i + 1] = uvrect.y + eps;
        terrainUV[8 * i + 2] = uvrect.x + uvrect.w - eps;
        terrainUV[8 * i + 3] = uvrect.y + eps;
        terrainUV[8 * i + 4] = uvrect.x + uvrect.w - eps;
        terrainUV[8 * i + 5] = uvrect.y + uvrect.h - eps;
        terrainUV[8 * i + 6] = uvrect.x + eps;
        terrainUV[8 * i + 7] = uvrect.y + uvrect.h - eps;
        i++;
    }
}

void WorldRenderer::render(World& world)
{
    switch(viewDirection)
    {
        case NORTH:
            renderNorth(world);
            break;
        case WEST:
            renderWest(world);
            break;
        case EAST:
            renderEast(world);
            break;
        case SOUTH:
            renderSouth(world);
            break;
    }
}

void WorldRenderer::renderNorth(World &world)
{
    // +I then -J
    int minRow = xi(screenX, screenY) / TERRAIN_TILE_SIZE - 1;
    int minCol = yj(screenX, screenY + WINDOW_H) / TERRAIN_TILE_SIZE - 1;
    int maxRow = xi(screenX, screenY + WINDOW_H) / TERRAIN_TILE_SIZE + 1;
    int maxCol = yj(screenX + WINDOW_W, screenY) / TERRAIN_TILE_SIZE + 1;
    for(int i = minRow; i < maxRow; i++)
    {
        for(int j = maxCol - 1; j >= minCol; j--)
        {
            float shade = RenderRoutines::calcTileShade(world.getHeight(i, j), world.getHeight(i + 1, j), world.getHeight(i + 1, j + 1), world.getHeight(i, j + 1));
            Point basePt = coord::project3DPoint(i * TERRAIN_TILE_SIZE, j * TERRAIN_TILE_SIZE, 0);
            color3f(shade, shade, shade);
            GROUND tileGround = world.getGround(i, j);
            texCoord2f(terrainUV[tileGround * 8], terrainUV[tileGround * 8 + 1]);
            vertex2i(basePt.x, basePt.y - world.getHeight(i, j));
            texCoord2f(terrainUV[tileGround * 8 + 2], terrainUV[tileGround * 8 + 3]);
            vertex2i(basePt.x + ISO_LENGTH / 2, basePt.y - ISO_WIDTH / 2 - world.getHeight(i, j + 1));
            texCoord2f(terrainUV[tileGround * 8 + 4], terrainUV[tileGround * 8 + 5]);
            vertex2i(basePt.x + ISO_LENGTH, basePt.y - world.getHeight(i + 1, j + 1));
            texCoord2f(terrainUV[tileGround * 8 + 6], terrainUV[tileGround * 8 + 7]);
            vertex2i(basePt.x + ISO_LENGTH / 2, basePt.y + ISO_WIDTH / 2 - world.getHeight(i + 1, j));
        }
    }
    RenderRoutines::drawString("N", 10, 10);
}

void WorldRenderer::renderWest(World &world)
{
    // +J then +I
    int minRow  = xi(screenX + WINDOW_W, screenY) / TERRAIN_TILE_SIZE - 1;
    int minCol = yj(screenX, screenY) / TERRAIN_TILE_SIZE - 1;
    int maxRow = xi(screenX, screenY + WINDOW_H) / TERRAIN_TILE_SIZE + 1;
    int maxCol = yj(screenX + WINDOW_W, screenY + WINDOW_H) / TERRAIN_TILE_SIZE + 1;
    for(int i = minRow; i < maxRow; i++)
    {
        for(int j = minCol; j < maxCol; j++)
        {
            float shade = RenderRoutines::calcTileShade(world.getHeight(i, j), world.getHeight(i + 1, j), world.getHeight(i + 1, j + 1), world.getHeight(i, j + 1));
            Point basePt = coord::project3DPoint(i * TERRAIN_TILE_SIZE, j * TERRAIN_TILE_SIZE, 0);
            color3f(shade, shade, shade);
            GROUND tileGround = world.getGround(i, j);
            texCoord2f(terrainUV[tileGround * 8], terrainUV[tileGround * 8 + 1]);
            vertex2i(basePt.x, basePt.y - world.getHeight(i, j));
            texCoord2f(terrainUV[tileGround * 8 + 2], terrainUV[tileGround * 8 + 3]);
            vertex2i(basePt.x + ISO_LENGTH / 2, basePt.y - ISO_WIDTH / 2 - world.getHeight(i, j + 1));
            texCoord2f(terrainUV[tileGround * 8 + 4], terrainUV[tileGround * 8 + 5]);
            vertex2i(basePt.x + ISO_LENGTH, basePt.y - world.getHeight(i + 1, j + 1));
            texCoord2f(terrainUV[tileGround * 8 + 6], terrainUV[tileGround * 8 + 7]);
            vertex2i(basePt.x + ISO_LENGTH / 2, basePt.y + ISO_WIDTH / 2 - world.getHeight(i + 1, j));

        }
    }
    RenderRoutines::drawString("W", 10, 10);
}

void WorldRenderer::renderEast(World &world)
{
    // -I then -J
    int minRow = xi(screenX + WINDOW_W, screenY + WINDOW_H) / TERRAIN_TILE_SIZE - 1;
    int minCol = yj(screenX, screenY + WINDOW_H) / TERRAIN_TILE_SIZE - 1;
    int maxRow = xi(screenX, screenY) / TERRAIN_TILE_SIZE + 1;
    int maxCol = yj(screenX + WINDOW_W, screenY) / TERRAIN_TILE_SIZE + 1;
    for(int i = maxRow - 1; i >= minRow; i--)
    {
        for(int j = maxCol - 1; j >= minCol; j--)
        {
            float shade = RenderRoutines::calcTileShade(world.getHeight(i, j), world.getHeight(i + 1, j), world.getHeight(i + 1, j + 1), world.getHeight(i, j + 1));
            Point basePt = coord::project3DPoint(i * TERRAIN_TILE_SIZE, j * TERRAIN_TILE_SIZE, 0);
            color3f(shade, shade, shade);
            GROUND tileGround = world.getGround(i, j);
            texCoord2f(terrainUV[tileGround * 8], terrainUV[tileGround * 8 + 1]);
            vertex2i(basePt.x, basePt.y - world.getHeight(i, j));
            texCoord2f(terrainUV[tileGround * 8 + 2], terrainUV[tileGround * 8 + 3]);
            vertex2i(basePt.x + ISO_LENGTH / 2, basePt.y - ISO_WIDTH / 2 - world.getHeight(i, j + 1));
            texCoord2f(terrainUV[tileGround * 8 + 4], terrainUV[tileGround * 8 + 5]);
            vertex2i(basePt.x + ISO_LENGTH, basePt.y - world.getHeight(i + 1, j + 1));
            texCoord2f(terrainUV[tileGround * 8 + 6], terrainUV[tileGround * 8 + 7]);
            vertex2i(basePt.x + ISO_LENGTH / 2, basePt.y + ISO_WIDTH / 2 - world.getHeight(i + 1, j));
            
        }
    }
    RenderRoutines::drawString("E", 10, 10);
}

void WorldRenderer::renderSouth(World &world)
{
    // -I then +J
    int minRow = xi(screenX + WINDOW_W, screenY + WINDOW_H) / TERRAIN_TILE_SIZE - 1;
    int maxRow = xi(screenX, screenY) / TERRAIN_TILE_SIZE + 1;
    int minCol = yj(screenX + WINDOW_W, screenY) / TERRAIN_TILE_SIZE - 1;
    int maxCol = yj(screenX, screenY + WINDOW_H) / TERRAIN_TILE_SIZE + 1;
    for(int i = maxRow - 1; i >= minRow; i--)
    {
        for(int j = minCol; j < maxCol; j++)
        {
            float shade = RenderRoutines::calcTileShade(world.getHeight(i, j), world.getHeight(i + 1, j), world.getHeight(i + 1, j + 1), world.getHeight(i, j + 1));
            Point basePt = coord::project3DPoint(i * TERRAIN_TILE_SIZE, j * TERRAIN_TILE_SIZE, 0);
            color3f(shade, shade, shade);
            GROUND tileGround = world.getGround(i, j);
            texCoord2f(terrainUV[tileGround * 8], terrainUV[tileGround * 8 + 1]);
            vertex2i(basePt.x, basePt.y - world.getHeight(i, j));
            texCoord2f(terrainUV[tileGround * 8 + 2], terrainUV[tileGround * 8 + 3]);
            vertex2i(basePt.x + ISO_LENGTH / 2, basePt.y - ISO_WIDTH / 2 - world.getHeight(i, j + 1));
            texCoord2f(terrainUV[tileGround * 8 + 4], terrainUV[tileGround * 8 + 5]);
            vertex2i(basePt.x + ISO_LENGTH, basePt.y - world.getHeight(i + 1, j + 1));
            texCoord2f(terrainUV[tileGround * 8 + 6], terrainUV[tileGround * 8 + 7]);
            vertex2i(basePt.x + ISO_LENGTH / 2, basePt.y + ISO_WIDTH / 2 - world.getHeight(i + 1, j));
            
        }
    }
    RenderRoutines::drawString("S", 10, 10);
}

void WorldRenderer::rotateLeft()
{
    //save previous tile coord, center x and top 1/3 in y
    double centerI = xi(screenX + WINDOW_W / 2, screenY + WINDOW_H / 3);
    double centerJ = yj(screenX + WINDOW_W / 2, screenY + WINDOW_H / 3);
    switch(viewDirection)
    {
        case NORTH:
            viewDirection = WEST;
            break;
        case WEST:
            viewDirection = SOUTH;
            break;
        case SOUTH:
            viewDirection = EAST;
            break;
        case EAST:
            viewDirection = NORTH;
            break;
    }
    int newScreenX = ix(centerI, centerJ) - WINDOW_W / 2;
    int newScreenY = jy(centerI, centerJ) - WINDOW_H / 3;
    screenX = newScreenX;
    screenY = newScreenY;
}

void WorldRenderer::rotateRight()
{
    double centerI = xi(screenX + WINDOW_W / 2, screenY + WINDOW_H / 3);
    double centerJ = yj(screenX + WINDOW_W / 2, screenY + WINDOW_H / 3);
    switch(viewDirection)
    {
        case NORTH:
            viewDirection = EAST;
            break;
        case EAST:
            viewDirection = SOUTH;
            break;
        case SOUTH:
            viewDirection = WEST;
            break;
        case WEST:
            viewDirection = NORTH;
            break;
    }
    int newScreenX = ix(centerI, centerJ) - WINDOW_W / 2;
    int newScreenY = jy(centerI, centerJ) - WINDOW_H / 3;
    screenX = newScreenX;
    screenY = newScreenY;
}

void WorldRenderer::panUp()
{
    if(xi(screenX, screenY) > 0 && yj(screenX + WINDOW_W, screenY) < TERRAIN_TILE_SIZE * WORLD_SIZE)
    {
        screenY -= PAN_SPEED;
    }
}

void WorldRenderer::panLeft()
{
    if(xi(screenX, screenY) > 0 && yj(screenX, screenY + WINDOW_H) > 0)
    {
        screenX -= PAN_SPEED;
    }
}

void WorldRenderer::panDown()
{
    if(yj(screenX, screenY + WINDOW_H) > 0 && xi(screenX + WINDOW_W, screenY + WINDOW_H) < TERRAIN_TILE_SIZE * WORLD_SIZE)
    {
        screenY += PAN_SPEED;
    }
}

void WorldRenderer::panRight()
{
    if(xi(screenX + WINDOW_W, screenY + WINDOW_H) < TERRAIN_TILE_SIZE * WORLD_SIZE && yj(screenX + WINDOW_W, screenY) < TERRAIN_TILE_SIZE * WORLD_SIZE)
    {
        screenX += PAN_SPEED;
    }
}

bool ptInScreen(Point p)
{
    if(p.x >= 0 && p.x < WINDOW_W && p.y >= 0 && p.y < WINDOW_H)
        return true;
    return false;
}

bool chunkInWorld(int x, int y)
{
    if(x >= 0 && x < WORLD_CHUNKS && y >= 0 && y < WORLD_CHUNKS)
        return true;
    return false;
}
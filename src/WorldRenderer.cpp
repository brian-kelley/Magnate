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

map<pair<int, int>, Chunk*> WorldRenderer::chunkCache;
//4 vertices, each with 2 floats
float terrainUV[GROUND::NUM_TYPES * 4 * 2];

bool ptInScreen(Point p);

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
    int sideLen = sqrt(MAX_CHUNK_CACHE);
    pair<int, int> screenPos = pixelToChunk(screenX, screenY);
    screenPos.first -= sideLen / 2;
    screenPos.second -= sideLen / 2;
    for(int i = screenPos.first; i < screenPos.first + sideLen - 1; i++)
    {
        for(int j = screenPos.second; j < screenPos.second + sideLen - 1; j++)
        {
            assertInCache(pair<int, int>(i, j));
        }
    }
}

void WorldRenderer::render()
{
    drawTerrain();
}

void WorldRenderer::trimChunkCache()
{
    if(chunkCache.size() > MAX_CHUNK_CACHE)
    {
        int curI = pixelToChunk(screenX, screenY).first;
        int curJ = pixelToChunk(screenX, screenY).second;
        for(auto i : chunkCache)
        {
            int distance = abs(curI - i.first.first) + abs(curJ - i.first.second);
            if(distance + 1 > sqrt(MAX_CHUNK_CACHE))
            {
                delete i.second;
                chunkCache.erase(i.first);
            }
        }
    }
}

void WorldRenderer::drawTerrain()
{
    glDisable(GL_SCISSOR_TEST);
    set<pair<int, int>> renderQueue;
    //test which chunks screen edge midpoints are in
    renderQueue.insert(pixelToChunk(screenX + WINDOW_W / 2, screenY));
    renderQueue.insert(pixelToChunk(screenX + WINDOW_W, screenY + WINDOW_H / 2));
    renderQueue.insert(pixelToChunk(screenX + WINDOW_W / 2, screenY + WINDOW_H + 256.0));
    renderQueue.insert(pixelToChunk(screenX, screenY + WINDOW_H / 2));
    //test corners
    renderQueue.insert(pixelToChunk(screenX, screenY));
    renderQueue.insert(pixelToChunk(screenX + WINDOW_W, screenY));
    renderQueue.insert(pixelToChunk(screenX + WINDOW_W, screenY + WINDOW_H + 256.0));
    renderQueue.insert(pixelToChunk(screenX, screenY + WINDOW_H + 256.0));
    //need to populate this value after checking if there's a 4-way chunk intersection onscreen
    pair<int, int> local = pixelToChunk(screenX, screenY);
    int minI = local.first + 3;
    int maxI = local.first - 3;
    int minJ = local.second + 3;
    int maxJ = local.second - 3;
    for(auto iter : renderQueue)
    {
        if(iter.first < minI)
        {
            minI = iter.first;
        }
        if(iter.first > maxI)
        {
            maxI = iter.first;
        }
        if(iter.second < minJ)
        {
            minJ = iter.second;
        }
        if(iter.second > maxJ)
        {
            maxJ = iter.second;
        }
    }
    //3 cases for # of rendered chunks: 1, 2 or 4
    if(minI == maxI && minJ == maxJ)       //Only one chunk onscreen
    {
        assertInCache(pair<int, int>(minI, minJ));
        drawChunk(chunkCache[pair<int, int>(minI, minJ)]);
    }
    else if(minI != maxI && minJ != maxJ) //Four chunks in a square
    {
        assertInCache(pair<int, int>(minI, minJ));
        assertInCache(pair<int, int>(minI, maxJ));
        assertInCache(pair<int, int>(maxI, minJ));
        assertInCache(pair<int, int>(maxI, maxJ));
        Chunk* east = chunkCache[pair<int, int>(maxI, maxJ)];
        Chunk* west = chunkCache[pair<int, int>(minI, minJ)];
        Chunk* north = chunkCache[pair<int, int>(minI, maxJ)];
        Chunk* south = chunkCache[pair<int, int>(maxI, minJ)];
        drawChunk(north);
        drawChunkBorder(north, west);
        drawChunkBorder(north, east);
        //center tile in 4-way intersection that isn't covered by the edges
        drawChunkIntersection(north, west, east, south);
        drawChunk(west);
        drawChunk(east);
        drawChunkBorder(west, south);
        drawChunkBorder(south, east);
        drawChunk(south);
    }
    else if(minI != maxI)   //two adjacent chunks that only differ in I
    {
        pair<int, int> point;
        point.first = minI;
        point.second = minJ;
        assertInCache(point);
        Chunk* topLeft = chunkCache[point];
        point.first++;
        assertInCache(point);
        Chunk* bottomRight = chunkCache[point];
        drawChunk(topLeft);
        drawChunkBorder(topLeft, bottomRight);
        drawChunk(bottomRight);
    }
    else if(minJ != maxJ)   //2 that only differ in J, I same for both
    {
        pair<int, int> point;
        point.first = minI;
        point.second = minJ;
        assertInCache(point);
        Chunk* bottomLeft = chunkCache[point];
        point.second++;
        assertInCache(point);
        Chunk* topRight = chunkCache[point];
        drawChunk(topRight);
        drawChunkBorder(bottomLeft, topRight);
        drawChunk(bottomLeft);
    }
}

//Draws every tile that can be drawn with only the nodes in this chunk (lacks 2 edges)
void WorldRenderer::drawChunk(Chunk* c)
{
    int baseX, baseY;
    Point left, right, top, bottom;
    const int TL = TERRAIN_TILE_SIZE * ISO_LENGTH / 2;
    const int TW = TERRAIN_TILE_SIZE * ISO_WIDTH / 2;
    for(int i = 0; i < Chunk::CHUNK_SIZE - 1; i++)
    {
        Point rowbase = coord::project3DPoint(c->getIOffset() + i * TERRAIN_TILE_SIZE, c->getJOffset(), 0);
        baseX = rowbase.x;
        baseY = rowbase.y;
        //this true when drawing the tiles, not just iterating over them
        bool rowStarted = false;
        for(int j = 0; j < Chunk::CHUNK_SIZE - 1; j++)
        {
            GROUND ng = c->mesh[i][j].g;
            left.x = baseX;
            left.y = baseY - c->mesh[i][j].height;
            top.x = baseX + TL;
            top.y = baseY - c->mesh[i][j + 1].height - TW;
            bottom.x = top.x;
            bottom.y = baseY - c->mesh[i + 1][j].height + TW;
            right.x = bottom.x + TL;
            right.y = baseY - c->mesh[i + 1][j + 1].height;
            baseX += TL;
            baseY -= TW;
            if(!rowStarted && right.x > 0 && (left.y < WINDOW_H + TW || right.y < WINDOW_H + TW || top.y < WINDOW_H + TW || bottom.y < WINDOW_H + TW))
            {
                rowStarted = true;
            }
            if(rowStarted)
            {
                //Now test whether
                if(left.x > WINDOW_W || (left.y < 0 && right.y < 0 && top.y < 0 && bottom.y < 0))
                {
                    break;
                }
                //Draw
                float tshade = RenderRoutines::calcTileShade(c->mesh[i][j].height, c->mesh[i + 1][j].height, c->mesh[i][j + 1].height, c->mesh[i + 1][j + 1].height);
                glColor3f(tshade, tshade, tshade);
                glBegin(GL_QUADS);
                glTexCoord2f(terrainUV[ng * 8], terrainUV[ng * 8 + 1]);
                glVertex2s(left.x, left.y);
                glTexCoord2f(terrainUV[ng * 8 + 2], terrainUV[ng * 8 + 3]);
                glVertex2s(top.x, top.y);
                glTexCoord2f(terrainUV[ng * 8 + 4], terrainUV[ng * 8 + 5]);
                glVertex2s(right.x, right.y);
                glTexCoord2f(terrainUV[ng * 8 + 6], terrainUV[ng * 8 + 7]);
                glVertex2s(bottom.x, bottom.y);
                glEnd();
            }
        }
    }
}

void WorldRenderer::drawChunkBorder(Chunk *c1, Chunk *c2)
{
    const int TL = TERRAIN_TILE_SIZE * ISO_LENGTH / 2;
    const int TW = TERRAIN_TILE_SIZE * ISO_WIDTH / 2;
    bool rowStarted = false;
    int baseX, baseY;
    Point left, right, top, bottom;
    /*
     Want this arrangement in space: (1st condition)
      2
     1
     or this: (2nd, else if {})
     1
      2
     */
    if(c1->getJ() != c2->getJ())
    {
        assert(c1->getI() == c2->getI());
        //Order c1, c2 so that c1 is southwest of c2
        if(c1->getJ() > c2->getJ())
        {
            Chunk* temp = c1;
            c1 = c2;
            c2 = temp;
        }
        Point base = coord::project3DPoint(c2->getIOffset(), c2->getJOffset() - TERRAIN_TILE_SIZE, 0);
        baseX = base.x;
        baseY = base.y;
        for(int i = 0; i < Chunk::CHUNK_SIZE; i++)
        {
            left.x = baseX;
            left.y = baseY - c1->mesh[i][Chunk::CHUNK_SIZE - 1].height;
            top.x = baseX + TL;
            top.y = baseY - c2->mesh[i][0].height - TW;
            bottom.x = top.x;
            bottom.y = baseY - c1->mesh[i + 1][Chunk::CHUNK_SIZE - 1].height + TW;
            right.x = top.x + TL;
            right.y = baseY - c2->mesh[i + 1][0].height;
            if(!rowStarted && right.x > 0 && (left.y > 0 || right.y > 0 || top.y > 0 || bottom.y > 0))
            {
                rowStarted = true;
            }
            if(rowStarted)
            {
                //break if row has gone offscreen
                if(left.x > WINDOW_W || (left.y > WINDOW_H && right.y > WINDOW_H && bottom.y > WINDOW_H && top.y > WINDOW_H))
                {
                    return;
                }
                float tshade = RenderRoutines::calcTileShade(c1->mesh[i][Chunk::CHUNK_SIZE - 1].height, c2->mesh[i][0].height, c2->mesh[i + 1][0].height, c1->mesh[i + 1][Chunk::CHUNK_SIZE - 1].height);
                GROUND ng = c1->mesh[i][Chunk::CHUNK_SIZE - 1].g;
                glColor3f(tshade, tshade, tshade);
                glBegin(GL_QUADS);
                glTexCoord2f(terrainUV[ng * 8], terrainUV[ng * 8 + 1]);
                glVertex2s(left.x, left.y);
                glTexCoord2f(terrainUV[ng * 8 + 2], terrainUV[ng * 8 + 3]);
                glVertex2s(top.x, top.y);
                glTexCoord2f(terrainUV[ng * 8 + 4], terrainUV[ng * 8 + 5]);
                glVertex2s(right.x, right.y);
                glTexCoord2f(terrainUV[ng * 8 + 6], terrainUV[ng * 8 + 7]);
                glVertex2s(bottom.x, bottom.y);
                glEnd();
            }
            base.x += TL;
            base.y += TW;
        }
    }
    else if(c1->getI() != c2->getI())
    {
        assert(c1->getJ() == c2->getJ());
        if(c1->getI() > c2->getI())
        {
            Chunk* temp = c1;
            c1 = c2;
            c2 = temp;
        }
        Point base = coord::project3DPoint(c2->getIOffset() - TERRAIN_TILE_SIZE, c2->getJOffset() + TERRAIN_TILE_SIZE * (Chunk::CHUNK_SIZE - 2), 0);
        baseX = base.x;
        baseY = base.y;
        for(int j = Chunk::CHUNK_SIZE - 2; j >= 0; j--)
        {
            left.x = baseX;
            left.y = baseY - c1->mesh[Chunk::CHUNK_SIZE][j].height;
            top.x = baseX + TL;
            top.y = baseY - c1->mesh[Chunk::CHUNK_SIZE][j + 1].height - TW;
            bottom.x = top.x;
            bottom.y = baseY - c2->mesh[0][j].height + TW;
            right.x = bottom.x + TL;
            right.y = baseY - c2->mesh[0][j + 1].height;
            if(!rowStarted && right.x > 0 && (left.y < WINDOW_H && right.y < WINDOW_H && bottom.y < WINDOW_H && top.y < WINDOW_H))
            {
                rowStarted = true;
            }
            if(rowStarted)
            {
                if(left.x > WINDOW_W || (left.y < 0 || right.y < 0 || top.y < 0 || bottom.y < 0))
                {
                    return;
                }
                float tshade = RenderRoutines::calcTileShade(c1->mesh[Chunk::CHUNK_SIZE - 1][j].height, c1->mesh[Chunk::CHUNK_SIZE - 1][j + 1].height, c2->mesh[0][j + 1].height, c2->mesh[0][j].height);
                GROUND ng = c1->mesh[Chunk::CHUNK_SIZE - 1][j].g;
                glColor3f(tshade, tshade, tshade);
                glBegin(GL_QUADS);
                glTexCoord2f(terrainUV[ng * 8], terrainUV[ng * 8 + 1]);
                glVertex2s(left.x, left.y);
                glTexCoord2f(terrainUV[ng * 8 + 2], terrainUV[ng * 8 + 3]);
                glVertex2s(top.x, top.y);
                glTexCoord2f(terrainUV[ng * 8 + 4], terrainUV[ng * 8 + 5]);
                glVertex2s(right.x, right.y);
                glTexCoord2f(terrainUV[ng * 8 + 6], terrainUV[ng * 8 + 7]);
                glVertex2s(bottom.x, bottom.y);
                glEnd();
            }
            baseX -= TL;
            baseY += TW;
        }
    }
}

void WorldRenderer::drawChunkIntersection(Chunk *c1, Chunk *c2, Chunk *c3, Chunk *c4)
{
    /*
     1
    2 3
     4
     */
    double tw = TERRAIN_TILE_SIZE * ISO_LENGTH / 2;
    double th = TERRAIN_TILE_SIZE * ISO_WIDTH / 2;
    GROUND ng = c2->mesh[Chunk::CHUNK_SIZE - 1][Chunk::CHUNK_SIZE - 1].g;
    float tshade = RenderRoutines::calcTileShade(c2->mesh[Chunk::CHUNK_SIZE - 1][Chunk::CHUNK_SIZE -1 ].height, c1->mesh[Chunk::CHUNK_SIZE - 1][0].height, c3->mesh[0][0].height, c4->mesh[0][Chunk::CHUNK_SIZE - 1].height);
    //Initialize to left first
    Point p = coord::project3DPoint(c3->getIOffset() - TERRAIN_TILE_SIZE, c3->getJOffset() - TERRAIN_TILE_SIZE, c2->mesh[Chunk::CHUNK_SIZE - 1][Chunk::CHUNK_SIZE - 1].height / ISO_HEIGHT);
    glColor3f(tshade, tshade, tshade);
    glBegin(GL_QUADS);
    glTexCoord2f(terrainUV[ng * 8], terrainUV[ng * 8 + 1]);
    glVertex2s(p.x, p.y);
    p.x += tw;
    p.y += c2->mesh[Chunk::CHUNK_SIZE - 1][Chunk::CHUNK_SIZE - 1].height - c1->mesh[Chunk::CHUNK_SIZE - 1][0].height - th;
    glTexCoord2f(terrainUV[ng * 8 + 2], terrainUV[ng * 8 + 3]);
    glVertex2s(p.x, p.y);
    p.x += tw;
    p.y += c1->mesh[Chunk::CHUNK_SIZE - 1][0].height - c3->mesh[0][0].height + th;
    glTexCoord2f(terrainUV[ng * 8 + 4], terrainUV[ng * 8 + 5]);
    glVertex2s(p.x, p.y);
    p.x -= tw;
    p.y += c3->mesh[0][0].height - c4->mesh[0][Chunk::CHUNK_SIZE - 1].height + th;
    glTexCoord2f(terrainUV[ng * 8 + 6], terrainUV[ng * 8 + 7]);
    glVertex2s(p.x, p.y);
    glEnd();
}

pair<int, int> WorldRenderer::pixelToChunk(int scrX, int scrY)
{
    pair<int, int> coords;
    coords.first = floor(xi(scrX, scrY) / (TERRAIN_TILE_SIZE * Chunk::CHUNK_SIZE));
    coords.second = floor(yj(scrX, scrY) / (TERRAIN_TILE_SIZE * Chunk::CHUNK_SIZE));
    return coords;
}

void WorldRenderer::assertInCache(pair<int, int> chunkLoc)
{
    if(chunkCache.find(chunkLoc) == chunkCache.end())
    {
        chunkCache[chunkLoc] = new Chunk(chunkLoc.first, chunkLoc.second);
    }
    trimChunkCache();

}

bool ptInScreen(Point p)
{
    if(p.x >= 0 && p.x < WINDOW_W && p.y >= 0 && p.y < WINDOW_H)
        return true;
    return false;
}

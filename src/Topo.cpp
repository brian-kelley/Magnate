#include "Topo.h"

using namespace std;
using namespace Coord;
using namespace GlobalConfig;

void Topo::generateTopo()
{
    auto heights = World::getHeights();
    const int STEP = (WORLD_SIZE / 2) / 15; // Height units between topo lines. WS / 2 is max height, so have 15 lines
    auto tex = Atlas::textureFromName("topo");
    int topoSize = tex.width;
    short* buf = new short[topoSize * topoSize];
    const double scl = double(WORLD_SIZE) / topoSize;
    for(int i = 0; i < topoSize; i++)
    {
        for(int j = 0; j < topoSize; j++)
        {
            buf[topoSize * j + i] = float(heights.get(i * scl, j * scl) + STEP / 2) / STEP;
        }
    }
    Color4* pixels = new Color4[topoSize * topoSize];
    const Color4 line(0, 0, 0, 255);     //opaque black
    const Color4 empty(0, 0, 0, 0);      //transparency
    for(int i = 0; i < topoSize; i++)
    {
        for(int j = 0; j < topoSize; j++)
        {
            //If a height in buf is different than all its neighbors and it is the least of its group, mark as a line
            pixels[i + j * topoSize] = empty;
            Pos2 loc(i, j);
            for(int dir = UP; dir <= RIGHT; dir++)
            {
                Pos2 inDir = getTileInDir(loc, dir);
                if(inDir.x >= 0 && inDir.x < topoSize && inDir.y >= 0 && inDir.y < topoSize)
                {
                    if(buf[i + topoSize * j] < buf[inDir.x + topoSize * inDir.y])
                    {
                        pixels[i + j * topoSize] = line;
                        break;
                    }
                }
            }
        }
    }
    Atlas::sendImage(pixels, Atlas::tileFromName("topo"));
    delete[] buf;
    delete[] pixels;
}
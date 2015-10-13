#include "Topo.h"

using namespace std;
using namespace constants;
using namespace RenderRoutines;
using namespace Coord;

void Topo::generateTopo()
{
    auto startTime = clock();
    const int STEP = 50; // Height units between topo lines
    int texID = texNumFromStr("topo");
    int topoSize = Atlas::tileW(texID);
    Height* buf = new Height[topoSize * topoSize];
    const int sampW = float(WORLD_SIZE) / topoSize; // Width of square sample area (in tiles) that forms each topo pxl
    for(int i = 0; i < topoSize; i++)
    {
        for(int j = 0; j < topoSize; j++)
        {
            int avgHeight = 0;
            for(int k = i * sampW; k < (i + 1) * sampW; k++)
            {
                for(int l = j * sampW; l < (j + 1) * sampW; l++)
                {
                    avgHeight += World::getHeight(k, l);
                }
            }
            avgHeight /= (sampW * sampW);
            //In buf round sample's average height to nearest step and set that to buf (not a color yet)
            buf[topoSize * j + i] = float(avgHeight + STEP / 2) / STEP;
        }
    }
    Uint32* pixels = new Uint32[topoSize * topoSize];
    const Uint32 line = getColor32(0, 0, 0);     //black
    const Uint32 empty = getColor32(0, 0, 0, 0); //transparency
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
                if(buf[i + topoSize * j] < buf[inDir.x + topoSize * inDir.y])
                {
                    pixels[i + j * topoSize] = line;
                    break;
                }
            }
        }
    }
    sendImage(pixels, "topo");
    delete[] buf;
    delete[] pixels;
    cout << "Topo map generation took " << (float(clock()) - startTime) / CLOCKS_PER_SEC << " seconds." << endl;
}

void Topo::drawTopo()
{
    int texID = texNumFromStr("topo");
    int x = WINDOW_W - Minimap::MINIMAP_BORDER - Minimap::MINIMAP_SIZE - BORDER_WIDTH;
    int y = Minimap::MINIMAP_BORDER + BORDER_WIDTH;
    int w = Minimap::MINIMAP_SIZE;
    blit(texID, x, y, x + w, y + w);
}
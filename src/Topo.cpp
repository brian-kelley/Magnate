#include "Topo.h"

using namespace std;
using namespace constants;
using namespace RenderRoutines;
using namespace Coord;

void Topo::generateTopo(Heightmap& heights)
{
    const int STEP = (WORLD_SIZE / 2) / 15; // Height units between topo lines. WS / 2 is max height, so have 15 lines
    int texID = texNumFromStr("topo");
    int topoSize = Atlas::tileW(texID);
    Height* buf = new Height[topoSize * topoSize];
    const double scl = double(WORLD_SIZE) / topoSize;
    for(int i = 0; i < topoSize; i++)
    {
        for(int j = 0; j < topoSize; j++)
        {
            buf[topoSize * j + i] = float(heights->get(i * scl, j * scl) + STEP / 2) / STEP;
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
    Atlas::sendImage((byte*) pixels, Atlas::tileFromName("topo"));
    delete[] buf;
    delete[] pixels;
}

void Topo::drawTopo()
{
    int texID = texNumFromStr("topo");
    int x = WINDOW_W - Minimap::MINIMAP_BORDER - Minimap::MINIMAP_SIZE - BORDER_WIDTH;
    int y = Minimap::MINIMAP_BORDER + BORDER_WIDTH;
    int w = Minimap::MINIMAP_SIZE;
    using namespace Renderer;
    disableTexture();
    color4b(255, 255, 255, 140);
    vertex2i(x, y);
    vertex2i(x + w, y);
    vertex2i(x + w, y + w);
    vertex2i(x, y + w);
    enableTexture();
    color4b(255, 255, 255, 255);
    blit(texID, x, y, x + w, y + w);
}
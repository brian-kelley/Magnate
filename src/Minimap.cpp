#include "Minimap.h"

using namespace std;
using namespace Coord;
using namespace glm;

int Minimap::MINIMAP_SIZE = 128;
int Minimap::MINIMAP_BORDER = 15;
mat4 Minimap::tileToMinimap;
mat4 Minimap::minimapToTile;

Minimap::Minimap(Scene* gameScene) : Component(gameScene->getScreenRect().w - MINIMAP_BORDER - MINIMAP_SIZE, MINIMAP_BORDER, MINIMAP_SIZE, MINIMAP_SIZE, StickyDirs::right | StickyDirs::top | StickyDirs::fixedWidth | StickyDirs::fixedHeight, false, gameScene)
{
    float MM_SCALE = float(MINIMAP_SIZE) / GlobalConfig::WORLD_SIZE;
    tileToMinimap = mat4();
    tileToMinimap = scale(tileToMinimap, {MM_SCALE, 1, MM_SCALE});
    minimapToTile = inverse(tileToMinimap);
    World::worldLoaded.addListener(nullptr, processWorldLoading);
}

Color4 Minimap::colorFromTerrain(Ground g)
{
    Color4 rv;
    switch(g)
    {
        case DESERT:
            rv.r = 0xDA;
            rv.g = 0xDB;
            rv.b = 0x6F;
            return rv;
        case RAINFOREST:
            rv.r = 0x00;
            rv.g = 0x80;
            rv.b = 0x10;
            return rv;
        case DECID_FOREST:
        case CONIFER_FOREST:
        case TAIGA:
            rv.r = 0x00;
            rv.g = 0x9A;
            rv.b = 0x2F;
            return rv;
        case MOUNTAINS:
        case STONE:
            rv.r = 0xA0;
            rv.g = 0xA0;
            rv.b = 0xA0;
            return rv;
        case PLAINS:
        case FLOODPLAINS:
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
        case SNOWCAP:
        case TUNDRA:
            rv.r = 0xD0;
            rv.g = 0xD0;
            rv.b = 0xD0;
            return rv;
        default:
            rv.r = 0xFF;
            rv.g = 0xFF;
            rv.b = 0xFF;
            return rv;
    }
}

void Minimap::putMinimapPixel(int x, int y, Color4* buf, int maxHeight)
{
    const auto& heights = World::getHeights();
    const auto& biomes = World::getBiomes();
    int compression = float(GlobalConfig::WORLD_SIZE) / Minimap::MINIMAP_SIZE;
    if(compression < 1)
        compression = 1;
    Pos2 tilePos;
    tilePos.x = x / compression;
    tilePos.y = y / compression;
    //array of occurrences of each terrain type
    int occurences[NUM_TYPES];
    for(int i = 0; i < NUM_TYPES; i++)
    {
        occurences[i] = 0;
    }
    int sumHeights = 0;
    for(int i = tilePos.x; i < tilePos.x + compression; i++)
    {
        for(int j = tilePos.y; j < tilePos.y + compression; j++)
        {
            int h = heights.get(i, j);
            occurences[biomes.get(i, j)]++;
            sumHeights += h;
        }
    }
    int avgHeight = sumHeights / (compression * compression);
    Ground commonest = WATER;
    for(int i = 0; i < NUM_TYPES; i++)
    {
        if(occurences[i] > occurences[commonest])
            commonest = (Ground) i;
    }
    if(occurences[RIVER] > 0)
        commonest = RIVER;
    else if(occurences[LAKE] > 0)
        commonest = LAKE;
    Color4 pxColor = colorFromTerrain(commonest);
    double heightComp = avgHeight / double(maxHeight);
    if(heightComp < 0)
        heightComp = 0;
    double colorMult = 0.8 + 0.6 * heightComp;
    pxColor *= colorMult;
    buf[x + y * MINIMAP_SIZE] = pxColor;
}

void Minimap::buildTexture()
{
    const auto& heights = World::getHeights();
    Color4* pixelData = new Color4[MINIMAP_SIZE * MINIMAP_SIZE];
    short maxHeight = 0;
    for(int i = 0; i < GlobalConfig::WORLD_SIZE; i++)
    {
        for(int j = 0; j < GlobalConfig::WORLD_SIZE; j++)
        {
            auto h = heights.get(i, j);
            if(h > maxHeight)
                maxHeight = h;
        }
    }
    for(int i = 0; i < MINIMAP_SIZE; i++)
    {
        for(int j = 0; j < MINIMAP_SIZE; j++)
        {
            putMinimapPixel(i, j, pixelData, maxHeight);
        }
    }
    Atlas::sendImage(pixelData, Atlas::tileFromName("minimap"));
    delete[] pixelData;
    //Return to drawing to normal front buffer, visible on screen
}

vec4 Minimap::mapToWorld(Pos2 mapPos)
{
    //map pixel -> tile -> world
    vec4 mapVec = {mapPos.x, 0, mapPos.y, 1};
    vec4 tileVec = minimapToTile * mapVec;
    return tileToWorld(tileVec.x, World::getHeights().get(tileVec.x, tileVec.z), tileVec.z);
}

CompType Minimap::getType()
{
    return CompType::minimap;
}

void Minimap::mouseButton(const SDL_MouseButtonEvent &event)
{
    if(Input::isMouseDown)
        setCam();
}

void Minimap::mouseMotion(const SDL_MouseMotionEvent &event)
{
    if(Input::isMouseDown)
        setCam();
}

void Minimap::setCam()
{
    auto pos = getLocalMouse();
    vec4 xz = mapToWorld(pos);
    xz.y = Camera::getPosition().y;
    Camera::moveToPos(xz);
}

void Minimap::processWorldLoading(void*, const bool& isLoaded)
{
    if(isLoaded)
    {
        buildTexture();
        Topo::generateTopo();
    }
}
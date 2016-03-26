#include "World.h"

#define USE_CACHED_TERRAIN true

using namespace std;
using namespace GlobalConfig;
using namespace boost::filesystem;

bool World::drawing = false;
Heightmap World::height;
Heightmap World::biomes;
string World::saveName;
unsigned World::seed;
Broadcaster<bool> World::worldLoaded;

void World::initDebug()
{
    init("asdf");
}

void World::init(std::string saveName)
{
    World::saveName = saveName;
    height.setSize(WORLD_SIZE, WORLD_SIZE);
    biomes.setSize(WORLD_SIZE, WORLD_SIZE);
    path saveFolder = FileIO::root() / "saves";
    if(!exists(saveFolder))
    {
        cout << "Fatal error: save folder doesn't exist where expected:" << endl;
        cout << absolute(saveFolder) << endl;
        exit(EXIT_FAILURE);
    }
    path wPath = saveFolder / string(saveName + ".mag");
    if(exists(wPath))
    {
        cout << "Will read existing world from disk." << endl;
        read();
    }
    else
    {
        cout << "Will generate and save new world." << endl;
        seed = RandomUtils::gen();
        //write out initial data (seed)
        write();
    }
    TerrainGen tg;
    drawing = true; //TODO: When to set and unset this depends on where GUI widgets are implemented
    worldLoaded.send(true);
}

void World::initCached(string name)
{
    path cache = FileIO::root() / "temp" / name;
    if(!USE_CACHED_TERRAIN)
    {
        PRINT("Terrain caching disabled.");
        init("asdf");
    }
    else if(!exists(cache))
    {
        PRINT("Will generate and save terrain.");
        init("asdf");
        FILE* f = fopen(cache.string().c_str(), "wb");
        fwrite(height.buf, 1, height.getByteSize(), f);
        fwrite(biomes.buf, 1, biomes.getByteSize(), f);
        PRINT("Wrote cached debug terrain.");
        return;
    }
    else
    {
        PRINT("Will load cached terrain.");
        //quickly load from the file straight into the heightmaps
        FILE* f = fopen(cache.string().c_str(), "rb");
        height.setSize(WORLD_SIZE, WORLD_SIZE);
        biomes.setSize(WORLD_SIZE, WORLD_SIZE);
        fread(height.buf, 1, height.getByteSize(), f);
        fread(biomes.buf, 1, biomes.getByteSize(), f);
        PRINT("Loaded cached terrain");
        fclose(f);
    }
}

void World::write()
{
    path outPath = FileIO::root() / "saves" / string(saveName + ".mag");
    FILE* os = fopen(outPath.c_str(), "wb");
    fwrite(&seed, sizeof(seed), 1, os);
    fclose(os);
    cout << "Wrote out world file to:" << endl;
    cout << outPath.string() << endl;
}

void World::read()
{
    path inPath = FileIO::root() / "saves" / (saveName + ".mag");
    FILE* is = fopen(inPath.c_str(), "rb");
    fread(&seed, sizeof(seed), 1, is);
    fclose(is);
}

void World::update()
{
    //Do all world simulation for one frame
    //TODO: double buffer all frequently updated world data for easy parallelism
}

void World::saveAndExit()
{
    worldLoaded.send(false);
}

Heightmap& World::getHeights()
{
    return height;
}

Heightmap& World::getBiomes()
{
    return biomes;
}

bool World::isDrawing()
{
    return drawing;
}

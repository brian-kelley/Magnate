#include "World.h"

using namespace std;
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
    PRINT("Done loading test world.");
}

void World::init(std::string saveName)
{
    World::saveName = saveName;
    {
        Heightmap temp(GlobalConfig::WORLD_SIZE, GlobalConfig::WORLD_SIZE);
        height = temp;
        biomes = temp;
    }
    path saveFolder = initial_path() / FileIO::root / "saves";
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
    TerrainGen tg(height, biomes);
    drawing = true; //TODO: When to set and unset this depends on where GUI widgets are implemented
    worldLoaded.send(true);
}

void World::write()
{
    path outPath = initial_path() / FileIO::root / "saves" / string(saveName + ".mag");
    FILE* os = fopen(outPath.c_str(), "wb");
    fwrite(&seed, sizeof(seed), 1, os);
    fclose(os);
    cout << "Wrote out world file to:" << endl;
    cout << outPath.string() << endl;
}

void World::read()
{
    path inPath = initial_path() / FileIO::root / "saves" / (saveName + ".mag");
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

const Heightmap& World::getHeights()
{
    return height;
}

const Heightmap& World::getBiomes()
{
    return biomes;
}

bool World::isDrawing()
{
    return drawing;
}
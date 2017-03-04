#include "World.h"

#define USE_CACHED_TERRAIN false

using namespace std;
using namespace FileIO;
using namespace GlobalConfig;
using namespace boost::filesystem;

bool World::drawing = false;
Heightmap World::height;
Heightmap World::biomes;
Mesh World::mesh;
string World::saveName;
unsigned World::seed;
Broadcaster<bool> World::worldLoaded;

void World::init(std::string saveName)
{
  World::saveName = saveName;
  height.setSize(WORLD_SIZE, WORLD_SIZE);
  biomes.setSize(WORLD_SIZE, WORLD_SIZE);
  path saveFolder = FileIO::root() / "saves";
  if(!fileExists(saveFolder))
  {
    cout << "Fatal error: save folder doesn't exist where expected:" << endl;
    cout << absolute(saveFolder) << endl;
    exit(EXIT_FAILURE);
  }
  path wPath = saveFolder / string(saveName + ".mag");
  if(fileExists(wPath))
  {
    read();                     //loads seed
  }
  else
  {
    seed = RandomUtils::gen();  //create a random seed
    write();
  }
  RandomUtils::seed(seed);
  if(USE_CACHED_TERRAIN)
  {
    initCached(saveName);
  }
  else
  {
    TerrainGen tg(height, biomes);
  }
  RandomUtils::seed(seed);
  mesh.initWorldMesh(height, biomes);
  drawing = true; //TODO: When to set and unset this depends on where GUI widgets are implemented
  worldLoaded.send(true);
  PRINT("World seed = " << seed);
}

void World::initCached(string name)
{
  path cache = FileIO::root() / "temp" / name;
  if(!fileExists(cache))
  {
    PRINT("Creating terrain cache.");
    TerrainGen tg(height, biomes);
    writeTerrainCache(name);
  }
  else
  {
    read();
    PRINT("Will read from terrain cache.");
    readTerrainCache(name);
  }
}

void World::write()
{
  path outPath = FileIO::root() / "saves" / string(saveName + ".mag");
  FILE* os = fopen(outPath.c_str(), "wb");
  if(!os)
  {
    PRINT("Could not open save file \"" << outPath << "\" for writing!");
    return;
  }
  fwrite(&seed, sizeof(seed), 1, os);
  fclose(os);
  cout << "Wrote out world file to:" << endl;
  cout << outPath.string() << endl;
}

void World::read()
{
  path inPath = FileIO::root() / "saves" / (saveName + ".mag");
  FILE* is = fopen(inPath.c_str(), "rb");
  if(!is)
  {
    PRINT("Could not open save file \"" << inPath << "\" for reading!");
    return;
  }
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

void World::readTerrainCache(string name)
{
  path p = FileIO::root() / "temp" / name;
  FILE* f = fopen(p.c_str(), "rb");
  if(!f)
  {
    PRINT("Error opening terrain cache file for reading!");
    init("asdf");
    return;
  }
  size_t num = WORLD_SIZE * WORLD_SIZE;
  height.setSize(WORLD_SIZE, WORLD_SIZE);
  biomes.setSize(WORLD_SIZE, WORLD_SIZE);
  for(size_t i = 0; i < num; i++)
  {
    fread(&height.buf[i], 2, 1, f);
  }
  for(size_t i = 0; i < num; i++)
  {
    fread(&biomes.buf[i], 2, 1, f);
  }
  fclose(f);
}

void World::writeTerrainCache(string name)
{
  path p = FileIO::root() / "temp" / name;
  FILE* f = fopen(p.c_str(), "wb");
  if(!f)
  {
    PRINT("Error opening terrain cache file for writing!");
    init("asdf");
    return;
  }
  size_t num = WORLD_SIZE * WORLD_SIZE;
  for(size_t i = 0; i < num; i++)
  {
    fwrite(&height.buf[i], 2, 1, f);
  }
  for(size_t i = 0; i < num; i++)
  {
    fwrite(&biomes.buf[i], 2, 1, f);
  }
  fclose(f);
}

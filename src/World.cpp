//
//  World
//  MagIndev
//
//  Created by Brian Kelley on 10/21/14294.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "World.h"

using namespace std;
using namespace boost::filesystem;
using namespace constants;

string World::currentSaveName = "";
unsigned long long int World::seed;
Chunk* World::chunks[WORLD_CHUNKS][WORLD_CHUNKS];

void World::init(std::string saveName, bool willGenerate)
{
    currentSaveName = saveName;
    for(int i = 0; i < WORLD_CHUNKS; i++)
    {
        for(int j = 0; j < WORLD_CHUNKS; j++)
        {
            chunks[i][j] = new Chunk();
        }
    }
    path saveFolder = initial_path() / constants::BIN_TO_ROOT / "saves";
    if(!exists(saveFolder))
    {
        cout << "Fatal error: save folder doesn't exist where expected:" << endl;
        cout << absolute(saveFolder) << endl;
        exit(EXIT_FAILURE);
    }
    path wPath = saveFolder / string(saveName + ".mag");
    if(willGenerate)
    {
        cout << "Will generate and save new world." << endl;
        seed = (unsigned long long)(RandomUtils::gen()) << 32 | RandomUtils::gen();
        writeWorld();
    }
    else
    {
        cout << "Will read existing world from disk." << endl;
        readWorld();
    }
    generate();
    Terrain::init();
    Minimap::buildTexture();
}

void World::dispose()
{
    for(int i = 0; i < WORLD_CHUNKS; i++)
    {
        for(int j = 0; j < WORLD_CHUNKS; j++)
        {
            delete chunks[i][j];
        }
    }
}

void World::generate()
{
    //Set the world's permanent 64-bit seed value
    cout << "Starting world generation." << endl;
    Terrain::init();
    TerrainGen::generate();
}

void World::writeWorld()
{
    ofstream os;
    path outPath = initial_path() / BIN_TO_ROOT / "saves" / string(currentSaveName + ".mag");
    os.open(outPath.string(), ofstream::out | ofstream::binary | ofstream::trunc);
    if(os.good())
    {
        os.write((char*) &seed, sizeof(seed));
    }
    else
    {
        cout << "Error writing file." << endl;
    }
    cout << "Wrote out world file to:" << endl;
    cout << outPath.string() << endl;
    os.close();
}

void World::readWorld()
{
    path inPath = initial_path() / BIN_TO_ROOT / "saves" / string(currentSaveName + ".mag");
    ifstream is(inPath.string(), ifstream::in | ifstream::binary);
    is.open(inPath.string());
    is.read((char*) &seed, sizeof(seed));
    is.close();
}

void World::setGround(GROUND ground, int wi, int wj)
{
    int ci = wi / CHUNK_SIZE;
    int cj = wj / CHUNK_SIZE;
    int ti = wi - CHUNK_SIZE * ci;
    int tj = wj - CHUNK_SIZE * cj;
    chunks[ci][cj]->terrain[ti][tj] = ground;
}

void World::setGround(GROUND ground, Pos2 loc)
{
    setGround(ground, loc.x, loc.y);
}

void World::setHeight(Height height, int wi, int wj)
{
    int ci = wi / CHUNK_SIZE;
    int cj = wj / CHUNK_SIZE;
    int ti = wi - CHUNK_SIZE * ci;
    int tj = wj - CHUNK_SIZE * cj;
    chunks[ci][cj]->mesh[ti][tj] = height;
}

void World::setHeight(Height height, Pos2 loc)
{
    setHeight(height, loc.x, loc.y);
}

Height World::getHeight(int wi, int wj)
{
    if(wi < 0 || wi >= WORLD_SIZE || wj < 0 || wj >= WORLD_SIZE)
        return 0;
    int ci = wi / CHUNK_SIZE;
    int cj = wj / CHUNK_SIZE;
    int ti = wi - CHUNK_SIZE * ci;
    int tj = wj - CHUNK_SIZE * cj;
    return chunks[ci][cj]->mesh[ti][tj];
}

Height World::getHeight(Pos2 loc)
{
    return World::getHeight(loc.x, loc.y);
}


GROUND World::getGround(int wi, int wj)
{
    if(wi < 0 || wi >= WORLD_SIZE || wj < 0 || wj >= WORLD_SIZE)
        return WATER;
    int ci = wi / CHUNK_SIZE;
    int cj = wj / CHUNK_SIZE;
    int ti = wi - CHUNK_SIZE * ci;
    int tj = wj - CHUNK_SIZE * cj;
    return chunks[ci][cj]->terrain[ti][tj];
}

GROUND World::getGround(Pos2 loc)
{
    return getGround(loc.x, loc.y);
}

bool World::tileInWorld(int x, int y)
{
    if(x >= 0 && x < WORLD_SIZE && y >= 0 && y < WORLD_SIZE)
        return true;
    else
        return false;
}

bool World::tileInWorld(Pos2 pos)
{
    if(pos.x >= 0 && pos.x < WORLD_SIZE && pos.y >= 0 && pos.y < WORLD_SIZE)
        return true;
    else
        return false;
}
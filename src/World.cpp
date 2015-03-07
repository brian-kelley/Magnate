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

World::World(std::string saveName, bool willGenerate)
{
    cout << willGenerate << endl;
    currentSaveName = saveName;
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
        generate();
        writeWorld();
    }
    else
    {
        cout << "Will read existing world from disk." << endl;
        readWorld();
    }
    cout << "Successfully ran World constructor." << endl;
}

World::~World()
{

}

void World::generate()
{
    //Set the world's permanent 64-bit seed value
    cout << "Starting world generation." << endl;
    seed = rand() + (uint64_t(rand()) << 32);
    populateTerrain();
    cout << "Done with world generation." << endl;
}

void World::populateTerrain()
{
    cout << "Starting terrain gen." << endl;
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            terrain[i][j].g = (GROUND) (rand() % GROUND::NUM_TYPES);
            terrain[i][j].height = rand();
        }
    }
    cout << "Done with terrain gen." << endl;
    cout << "Starting terrain smoothing (first iteration)" << endl;
    int sum;
    int n;
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            sum = terrain[i][j].height;
            n = 1;
            if(i > 0)
            {
                sum += terrain[i - 1][j].height;
                n++;
            }
            if(i < WORLD_SIZE - 1)
            {
                sum += terrain[i + 1][j].height;
                n++;
            }
            if(j > 0)
            {
                sum += terrain[i][j - 1].height;
                n++;
            }
            if(j < WORLD_SIZE - 1)
            {
                sum += terrain[i][j + 1].height;
                n++;
            }
            terrain[i][j].height = sum / n;
        }
    }
    cout << "Finishing first terrain smoothing." << endl;
}

void World::writeWorld()
{
    ofstream os;
    path outPath = initial_path() / BIN_TO_ROOT / "saves" / string(currentSaveName + ".mag");
    os.open(outPath.string(), ofstream::out | ofstream::binary | ofstream::trunc);
    //Write terrain data first
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(os.good())
            {
                os << terrain[i][j].g;
            }
            else
            {
                cout << "File writing error." << endl;
                cout << "Is save file damaged?" << endl;
                os.close();
                return;
            }
        }
    }
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(os.good())
            {
                 os << terrain[i][j].height;
            }
            else
            {
                cout << "File writing error." << endl;
                cout << "Is save file damaged?" << endl;
                os.close();
                return;
            }
        }
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
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(is.good())
            {
                terrain[i][j].g = (GROUND) is.get();
            }
            else
            {
                cout << "File reading error." << endl;
                cout << "Is save file damaged?" << endl;
                is.close();
                return;
            }
        }
    }
    for(int i = 0; i < WORLD_SIZE; i++)
    {
        for(int j = 0; j < WORLD_SIZE; j++)
        {
            if(is.good())
            {
                terrain[i][j].height = (unsigned char) is.get();
            }
            else
            {
                cout << "File reading errror." << endl;
                cout << "Is save file damaged?" << endl;
                is.close();
                return;
            }
        }
    }
}

GROUND World::getTerrainType(int x, int y)
{
    return terrain[y][x].g;
}

int World::getTerrainHeight(int x, int y)
{
    return terrain[y][x].height;
}

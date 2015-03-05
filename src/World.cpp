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
    path saveFolder = initial_path() / constants::BIN_TO_ROOT / "saves";
    if(!exists(saveFolder))
    {
        cout << "Fatal error: save folder doesn't exist where expected:" << endl;
        cout << absolute(saveFolder) << endl;
        exit(EXIT_FAILURE);
    }
    path wPath;
    if(saveName.substr(saveName.size() - 4, 4) != ".mag")
    {
        wPath = saveFolder / string(saveName + ".mag");
    }
    else
    {
        wPath = saveFolder / saveName;
    }
    if(!exists(wPath))
    {
        
    }
    if(willGenerate)
    {
        generate();
        writeWorld();
    }
    else
    {
        readWorld();
    }
}

World::~World()
{
    
}

void World::generate()
{
    //Set the world's permanent 64-bit seed value
    cout << "Starting world generation." << endl;
    seed = rand() + (long(rand()) << 32);
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

void writeWorld()
{
    
}

void readWorld()
{
    
}
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
    Terrain::init(seed);
    
}

World::~World()
{

}

void World::generate()
{
    //Set the world's permanent 64-bit seed value
    cout << "Starting world generation." << endl;
    seed = rand() + (uint64_t(rand()) << 32);
    Terrain::init(seed);
    cout << "Done with world generation." << endl;
}

void World::writeWorld()
{
    ofstream os;
    path outPath = initial_path() / BIN_TO_ROOT / "saves" / string(currentSaveName + ".mag");
    os.open(outPath.string(), ofstream::out | ofstream::binary | ofstream::trunc);
    if(os.good())
    {
        os << seed;
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
    is >> seed;
    is.close();
}
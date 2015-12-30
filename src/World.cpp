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

World::World(std::string saveName, bool willGenerate)
{
    this->saveName = saveName;
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
        seed = RandomUtils::gen();
        //write out initial data (seed)
        write();
    }
    else
    {
        cout << "Will read existing world from disk." << endl;
        read();
    }
    TerrainGen::generate(height, ground);
    Terrain::init();
    Minimap::buildTexture(height, ground);
    Topo::generateTopo(height);
}

void World::write()
{
    path outPath = initial_path() / BIN_TO_ROOT / "saves" / string(saveName + ".mag");
    FILE* os = fopen(outPath.c_str(), "wb");
    fwrite(&seed, sizeof(seed), 1, os);
    fclose(os);
    cout << "Wrote out world file to:" << endl;
    cout << outPath.string() << endl;
}

void World::read()
{
    path inPath = initial_path() / BIN_TO_ROOT / "saves" / (saveName + ".mag");
    FILE* is = fopen(inPath.c_str(), "rb");
    fread(&seed, sizeof(seed), 1, is);
    fclose(is);
}
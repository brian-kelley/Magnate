//
//  SaveManager.cpp
//  Magnate
//
//  Created by Brian Kelley on 1/22/1522.
//  Copyright (c) 2015 Brian Kelley. All rights reserved.
//

#include "SaveManager.h"

using namespace std;
using namespace boost::filesystem;

World* SaveManager::loadWorld(string worldFolder)
{
    /* TODO: When world structure is implemented, allocate memory for it
     and load it from file. */
    return nullptr;
}

void SaveManager::deleteWorld(string worldFolder)
{
    path wPath = initial_path() / ".." / "saves" / worldFolder;
    if(exists(wPath))
    {
        if(!boost::filesystem::remove(wPath))
        {
            cout << "Error: Failed to delete world: " << worldFolder << endl;
        }
    }
}

vector<string> SaveManager::listSaveFolders()
{
    vector<string> out;
    path saves = initial_path() / ".."/ "saves";
    out.push_back("not really a save");
    return out;
}
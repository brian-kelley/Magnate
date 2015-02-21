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

SaveManager::SaveManager()
{
    refreshSaveList();
}

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

void SaveManager::refreshSaveList()
{
    vector<string> out;
    path saveFolder = initial_path() / ".." / "saves";
    if(is_directory(saveFolder))
    {
        directory_iterator dirIter(saveFolder);
        directory_iterator end;
        while(dirIter != end)
        {
            if(is_directory(dirIter->path()))
            {
                saves.push_back(((dirIter->path()).stem()).string());
            }
            dirIter++;
        }
    }
}

vector<string>& SaveManager::listSaves()
{
    return saves;
}

int SaveManager::getNumSaves()
{
    return int(saves.size());
}

Scene* SaveManager::getScene()
{
    return &menu;
}

void SaveManager::initMenu(callback_t toMain, callback_t toGame)
{
    new Button(160, 430, 200, 80, "Back", toMain, &menu);
    new Button(480, 430, 200, 80, "Start", toGame, &menu);
    new ScrollBlock(320, 220, 550, 300, &menu, saves.size() * 50);
}
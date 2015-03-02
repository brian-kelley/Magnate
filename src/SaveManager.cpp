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
using namespace SaveManager;

Scene* SaveManager::menu;
Scene* SaveManager::renaming;
Scene* SaveManager::creatingNew;
Scene* SaveManager::deleting;
Field* SaveManager::renamingField;
Field* SaveManager::namingNewField;
MultiSelect* SaveManager::saveSelect;
Label* SaveManager::creatingNewWarning;
Label* SaveManager::renamingWarning;
Label* SaveManager::deletingName;
Scene** SaveManager::currentScenePtr;
vector<string> SaveManager::saves;

void SaveManager::init(Scene** currentSceneArg, callback_t toMain, callback_t toGame)
{
    refreshSaveList();
    currentScenePtr = currentSceneArg;
    initUI(toMain, toGame);
}

void SaveManager::initUI(callback_t toMain, callback_t toGame)
{
    menu = new Scene();
    renaming = new Scene();
    creatingNew = new Scene();
    deleting = new Scene();
    new Label(320, 40, 200, 60, "Select a save file", menu);
    new Button(64, 430, 110, 60, "Back", toMain, menu);
    new Button(192, 430, 110, 60, "Rename", enterRename, menu);
    new Button(320, 430, 110, 60, "Create", createNew, menu);
    new Button(448, 430, 110, 60, "Load", toGame, menu);
    new Button(576, 430, 110, 60, "Delete", deleteWorld, menu);
    ScrollBlock* listSB = new ScrollBlock(320, 220, 550, 300, menu, 1);
    saveSelect = new MultiSelect(constants::PAD, constants::PAD, listSB->getDrawRect().w - 3 * constants::PAD - constants::BAR_WIDTH, 280, 50, listSB, false);
    for(int i = 0; i < int(saves.size()); i++)
    {
        saveSelect->addOption(saves[i]);
    }
    listSB->updateCanvasHeight(saveSelect->getNumOptions() * 50 + constants::PAD * 2);
    renamingField = new Field(320, 100, 600, 50, "", nullptr, renaming);
    new Button(250, 250, 100, 50, "OK", renameOK, renaming);
    new Button(390, 250, 100, 50, "Cancel", renameCancel, renaming);
    new Button(250, 250, 100, 50, "OK", createNewOK, creatingNew);
    new Button(390, 250, 100, 50, "Cancel", createNewCancel, creatingNew);
    namingNewField = new Field(320, 100, 600, 50, "", nullptr, creatingNew);
    new Button(400, 300, 100, 50, "Yes", deleteOK, deleting);
    new Button(240, 300, 100, 50, "Cancel", deleteCancel, deleting);
    new Label(320, 100, 630, 50, "Are you sure you want to delete", deleting);
    deletingName = new Label(320, 150, 630, 50, "", deleting);
}

void SaveManager::disposeUI()
{
    delete menu;
    delete renaming;
    delete creatingNew;
    delete deleting;
}

World* SaveManager::loadWorld(string worldFolder)
{
    /* TODO: When world structure is implemented, allocate memory for it
     and load it from file. */
    return nullptr;
}

void SaveManager::refreshSaveList()
{
    vector<string> out;
    path saveFolder = initial_path() / constants::BIN_TO_ROOT / "saves";
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
    return menu;
}

void SaveManager::enterRename(void* arg)
{
    if(saveSelect->getSelection() != -1)
    {
        renamingField->setText(saveSelect->getSelectionText());
        renamingField->activate();
        *currentScenePtr = renaming;
    }
}

void SaveManager::renameCancel(void *arg)
{
    //Return without doing anything with the filename.
    if(renamingField->isActive())
    {
        renamingField->deactivate();
    }
    *currentScenePtr = menu;
}

void SaveManager::renameOK(void* arg)
{
    path renamingPath = initial_path() / constants::BIN_TO_ROOT / "saves" / saveSelect->getSelectionText();
    if(renamingField->isActive())
    {
        renamingField->deactivate();
    }
    string newName = renamingField->getText();
    if(strpbrk(newName.c_str(), "\\/:*?\"<>|") != NULL)
    {
        new Label(320, 200, 300, 70, "\\/:*?\"<>| not allowed in save file names.", renaming);
    }
    else if(newName != "")
    {
        //Actually do the filesystem action of renaming the folder
        boost::filesystem::rename(renamingPath, initial_path() / constants::BIN_TO_ROOT / "saves" / newName);
        refreshSaveList();
        if(saveSelect->getSelection() != -1)
        {
            saveSelect->getSelectionText() = newName;
        }
        *currentScenePtr = menu;
    }
}

void SaveManager::createNew(void *arg)
{
    namingNewField->setText("");
    namingNewField->activate();
    *currentScenePtr = creatingNew;
}

void SaveManager::createNewCancel(void *arg)
{
    if(creatingNew->isActive())
    {
        creatingNew->deactivate();
    }
    *currentScenePtr = menu;
}

void SaveManager::createNewOK(void *arg)
{
    if(namingNewField->isActive())
    {
        namingNewField->deactivate();
    }
    string newName = namingNewField->getText();
    if(newName.size() != 0)
    {
        if(strpbrk(newName.c_str(), "\\/:*?\"<>|") != NULL)
        {
            new Label(320, 200, 300, 70, "\\/:*?\"<>| not allowed in save file names.", renaming);
        }
        else
        {
            boost::filesystem::create_directory(initial_path() / constants::BIN_TO_ROOT / "saves" / newName);
            refreshSaveList();
            saveSelect->addOption(newName);
            ((ScrollBlock*) saveSelect->getParent())->matchCanvasToContents();
        }
    }
    *currentScenePtr = menu;
}

void SaveManager::deleteWorld(void *arg)
{
    if(saveSelect->getSelection() != -1)
    {
        deletingName->updateText("\"" + saveSelect->getSelectionText() + "\"?");
        *currentScenePtr = deleting;
    }
}

void SaveManager::deleteOK(void* arg)
{
    //Delete the folder
    path deletePath = initial_path() / constants::BIN_TO_ROOT / "saves" / saveSelect->getSelectionText();
    try
    {
        boost::filesystem::remove(deletePath);
    }
    catch (boost::filesystem::filesystem_error fe)
    {
        *currentScenePtr = menu;
        return;
    }
    refreshSaveList();
    int selectRemove = saveSelect->findSelection(saveSelect->getSelectionText());
    if(selectRemove != -1)
    {
        saveSelect->removeOption(selectRemove);
        saveSelect->clearSelection();
        ((ScrollBlock*) saveSelect->getParent())->matchCanvasToContents();
    }
    *currentScenePtr = menu;
}

void SaveManager::deleteCancel(void* arg)
{
    *currentScenePtr = menu;
}
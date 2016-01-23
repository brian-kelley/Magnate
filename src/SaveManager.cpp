#include "SaveManager.h"

using namespace std;
using namespace boost::filesystem;
using namespace SaveManager;

vector<string> SaveManager::saves;

void SaveManager::refreshSaveList()
{
    saves.clear();
    path saveFolder = initial_path() / FileIO::root / "saves";
    //Ensure that saves folder exists and is a directory
    if(!is_directory(saveFolder))
        create_directories(saveFolder);
    directory_iterator dirIter(saveFolder);
    directory_iterator end;
    while(dirIter != end)
    {
        if(!is_directory(dirIter->path()) && dirIter->path().stem() != "")
            saves.push_back(((dirIter->path()).stem()).string());
        dirIter++;
    }
}


void SaveManager::loadWorld(string saveName)
{
    path worldPath = initial_path() / FileIO::root / "saves" / (saveName + ".mag");
    if(exists(worldPath))
        cout << "World file already exists, will read." << endl;
    else
        cout << "World file does not exist, will be created." << endl;
    World::init(saveName, !exists(worldPath));
}

void SaveManager::renameSave(string newName, string oldName)
{
    path oldPath = initial_path() / FileIO::root / "saves" / string(oldName + ".mag");
    path newPath = initial_path() / FileIO::root / "saves" / string(newName + ".mag");
    boost::filesystem::rename(oldPath, newPath);
}

void SaveManager::deleteSave(string name)
{
    path deletePath = initial_path() / FileIO::root / "saves" / string(name + ".mag");
    try
    {
        boost::filesystem::remove(deletePath);
    }
    catch (boost::filesystem::filesystem_error& e)
    {
        DBASSERT(false);
    }
}
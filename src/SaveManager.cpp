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
    checkSaveFolder();
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
    checkSaveFolder();
    path worldPath = initial_path() / FileIO::root / "saves" / (saveName + ".mag");
    World::init(saveName);
}

void SaveManager::renameSave(string newName, string oldName)
{
    checkSaveFolder();
    path oldPath = initial_path() / FileIO::root / "saves" / string(oldName + ".mag");
    path newPath = initial_path() / FileIO::root / "saves" / string(newName + ".mag");
    boost::filesystem::rename(oldPath, newPath);
}

void SaveManager::deleteSave(string name)
{
    checkSaveFolder();
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

void SaveManager::checkSaveFolder()
{
    path saveFolder = initial_path() / FileIO::root / "saves";
    if(!exists(saveFolder))
    {
        try
        {
            create_directories(saveFolder);
        }
        catch (exception& e)
        {
            cout << "Fatal error: can't create save folder." << endl;
            abort();
        }
    }
}
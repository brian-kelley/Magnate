#ifndef __SAVE_MANAGER_H__
#define __SAVE_MANAGER_H__

#include <stdio.h>
#include <string>
#include <vector>
#include "FileIO.h"
#include "World.h"
#include "Scene.h"
#include "Button.h"
#include "ScrollBlock.h"
#include "MultiSelect.h"
#include "GUI.h"
#include "DebugTools.h"

namespace SaveManager
{
    void refreshSaveList();
    void renameSave(std::string newName, std::string oldName);
    void deleteSave(std::string name);
    void loadWorld(std::string saveName);
    void checkSaveFolder();
    extern std::vector<std::string> saves;
};

#endif
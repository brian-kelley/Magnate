//
//  SaveManager.h
//  Magnate
//
//  Created by Brian Kelley on 1/22/1522.
//  Copyright (c) 2015 Brian Kelley. All rights reserved.
//

#ifndef __Magnate__SaveManager__
#define __Magnate__SaveManager__

#include <stdio.h>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include "World.h"
#include "Scene.h"
#include "Button.h"
#include "ScrollBlock.h"
#include "MultiSelect.h"

namespace SaveManager
{
    //User interface elements
    extern Scene* menu;
    extern Scene* renaming;
    extern Scene* creatingNew;
    extern Scene* deleting;
    extern Field* renamingField;
    extern Field* namingNewField;
    extern MultiSelect* saveSelect;
    extern Label* creatingNewWarning;
    extern Label* renamingWarning;
    extern Label* deletingName;
    extern World* loadedWorld;
    //Other variables
    extern Scene** currentScenePtr;
    extern std::vector<std::string> saves;
    //Functions
    extern void init(Scene** currentScenePtr, callback_t toMain, callback_t toGame);
    extern void initUI(callback_t toMain, callback_t toGame);
    extern void disposeUI();
    extern void refreshSaveList();
    extern std::vector<std::string>& listSaves();
    extern World* getWorld();
    extern int getNumSaves();
    extern Scene* getScene();
    extern void enterRename(void* arg);
    extern void renameCancel(void* arg);
    extern void renameOK(void* arg);
    extern void createNew(void* arg);
    extern void createNewCancel(void* arg);
    extern void createNewOK(void* arg);
    extern void deleteWorld(void* arg);
    extern void deleteOK(void* arg);
    extern void deleteCancel(void* arg);
    extern void enterRename();
    extern void loadWorldBtn(void* arg);
    extern callback_t transitionToGame;
};

#endif /* defined(__Magnate__SaveManager__) */
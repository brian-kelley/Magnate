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
#include "World.h"
#include "Scene.h"
#include "Button.h"
#include "ScrollBlock.h"
#include "MultiSelect.h"

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

namespace SaveManager
{
    extern void init();
    extern void refreshSaveList();
    extern std::vector<std::string>& listSaves();
    extern World* loadWorld(std::string worldFolder);
    extern void deleteWorld(std::string worldFolder);
    extern void initMenu(callback_t toMain, callback_t toGame);
    extern int getNumSaves();
    extern Scene* getScene();
    extern std::vector<std::string> saves;
    extern Scene* menu;
    extern void rename(void* arg);
    extern MultiSelect* saveSelect;
};

#endif /* defined(__Magnate__SaveManager__) */
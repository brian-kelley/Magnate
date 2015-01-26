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

#ifdef __APPLE__
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#elif _WIN32

#elif __linux

#endif

class SaveManager
{
public:
    SaveManager();
    ~SaveManager();
    void refreshSaveList();
    std::vector<std::string>& listSaves();
    World* loadWorld(std::string worldFolder);
    void deleteWorld(std::string worldFolder);
    int getNumSaves();
private:
    std::vector<std::string> saves;
};

#endif /* defined(__Magnate__SaveManager__) */
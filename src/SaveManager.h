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

#ifdef __APPLE__
#include <boost/filesystem.hpp>
#elif _WIN32

#elif __linux

#endif

namespace SaveManager
{
    World* loadWorld(std::string worldFolder);
    void deleteWorld(std::string worldFolder);
    std::vector<std::string> listSaveFolders();
}

#endif /* defined(__Magnate__SaveManager__) */

//
//  DirManager.h
//  MagIndev
//
//  Created by Brian Kelley on 10/21/14294.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __MagIndev__DirManager__
#define __MagIndev__DirManager__

#include <stdexcept>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "Scene.h"

namespace DirManager
{
    struct buttonData_t
    {
        std::string text;
        int x;
        int y;
        int width;
        int height;
    };
    struct tileData_t
    {
        std::string name;
        float x;
        float y;
        float width;
        float height;
    };
    extern std::string dirPath;
    extern std::string sepChar;
    extern std::string delim(unsigned long& index, std::string& data);
    extern int delimInt(unsigned long& index, std::string& data);
    extern void getNextLine(std::ifstream& file, std::string& data);
    extern void findPath();
    extern std::string getPath();
    extern std::vector<tileData_t*> parseTiles(std::string path, float size);
    extern std::vector<std::string> exec(std::string cmd);
    extern std::vector<std::string> listSaves();
};

#endif

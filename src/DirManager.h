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

class DirManager
{
public:
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
private:
    static std::string dirPath;
    static std::string delim(unsigned long& index, std::string& data);
    static int delimInt(unsigned long& index, std::string& data);
    static Scene* parseScene(std::string fname);
    static Button* parseButton(std::ifstream& file);
    static Label* parseLabel(std::ifstream& file);
    static Field* parseField(std::ifstream& file);
    static void getNextLine(std::ifstream& file, std::string& data);
public:
    static void findPath();
    static std::string getPath();
    static std::vector<tileData_t> parseTiles(std::string path, float size);
    static std::vector<Scene*>* parseScenes();
    static std::vector<std::string>* exec(std::string cmd);
};

#endif

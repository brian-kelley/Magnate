//
//  Atlas.h
//  MagIndev
//
//  Created by Brian Kelley on 10/21/14294.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __MagIndev__Atlas__
#define __MagIndev__Atlas__

#ifdef __APPLE__
#include <gl.h>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#elif __linux
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#elif _WIN32
#include <GL\gl.h>
#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>
#endif

#include <stdio.h>
#include <iostream>
#include <string>
#include <ios>
#include <fstream>
#include <vector>
#include <map>
#include <stdexcept>
#include "DirManager.h"
#include "Constants.h"

class Atlas
{
public:
    Atlas(std::string imgDir, SDL_Renderer* renderer, int size = constants::DEFAULT_ATLAS_SIZE);
    ~Atlas();
    int tileFromName(std::string tilename);
    int tileFromChar(char c);
    void bind();
    inline float tileX(int index)
    {
        return this->tiles[index]->x;
    }
    inline float tileY(int index)
    {
        return this->tiles[index]->y;
    }
    inline float tileW(int index)
    {
        return this->tiles[index]->width;
    }
    inline float tileH(int index)
    {
        return this->tiles[index]->height;
    }
private:
    SDL_Texture* tex;
    int width;
    int height;
    struct named_tex_t
    {
        SDL_Surface* surface;
        std::string name;
    };
    std::map<std::string, int> tileNames;
    std::map<char, int> charTiles;
    std::vector<DirManager::tileData_t*> tiles;
    void buildAtlas(std::string imgDir, SDL_Renderer* renderer, int size);
    int delimInt(unsigned long& index, std::string& data);
    std::string delim(unsigned long& index, std::string& data);
    void sortForHeight(std::vector<named_tex_t*>& vec);
    void initCharTiles();
};

#endif

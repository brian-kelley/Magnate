//
//  Atlas.h
//  MagIndev
//
//  Created by Brian Kelley on 10/21/14294.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __MagIndev__Atlas__
#define __MagIndev__Atlas__
//Platform-specific includes
#ifdef __APPLE__
#include "gl.h"
#include "SDL.h"
#include "SDL_image.h"
#include <boost/filesystem.hpp>
#elif __linux
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#elif _WIN32
#include <GL/gl.h>
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
#include "Constants.h"

class Atlas
{
public:
    Atlas(std::string imageName, SDL_Renderer* renderer); //example, pass either "main" or "main.png"
    ~Atlas();
    int tileFromName(std::string tilename);
    int tileFromChar(char c);
    void bind();
    inline float tileX(int index)
    {
        return this->tiles[index].x;
    }
    inline float tileY(int index)
    {
        return this->tiles[index].y;
    }
    inline float tileW(int index)
    {
        return this->tiles[index].width;
    }
    inline float tileH(int index)
    {
        return this->tiles[index].height;
    }
private:
    SDL_Texture* tex;
    int size;
    struct named_tex_t
    {
        SDL_Surface* surface;
        std::string name;
    };
    std::map<std::string, int> tileNames;
    std::map<char, int> charTiles;
    std::vector<tileData_t> tiles;
    void buildAtlas(std::string imgDir, SDL_Renderer* renderer, int size);
    void sortForHeight(std::vector<named_tex_t*>& vec);
    void initCharTiles();
    void parseTiles(boost::filesystem::path fpath);
};

#endif

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
#include <boost/filesystem.hpp>
#elif _WIN32
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <boost/filesystem.hpp>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <ios>
#include <fstream>
#include <vector>
#include <map>
#include <stdexcept>
#include "Constants.h"

typedef struct
{
    std::string name;
    unsigned short x;
    unsigned short y;
    unsigned short width;
    unsigned short height;
} Texture;

namespace Atlas
{
    void init(std::string imageName, SDL_Renderer* renderer); //example, pass either "main"
    int tileFromName(std::string tilename);
    int tileFromChar(char c);
    unsigned short tileX(int index);
    unsigned short tileY(int index);
    unsigned short tileW(int index);
    unsigned short tileH(int index);
    void buildAtlas(std::string imgDir, SDL_Renderer* renderer, int size);
    void initCharTiles();
    void parseTiles(boost::filesystem::path fpath);
    GLuint getTextureID();
    extern std::map<std::string, int> tileNames;
    extern std::map<char, int> charTiles; //glyph textures
    extern std::vector<Texture> tiles;
    extern GLuint textureID;
};

#endif

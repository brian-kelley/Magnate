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
#include <iostream>
#include <string>
#include <ios>
#include <fstream>
#include <vector>
#include <map>
#include <stdexcept>
#include "Constants.h"
#include "GenTypes.h"
#include "DebugTools.h"

typedef struct
{
    std::string name;
    short x;
    short y;
    short width;
    short height;
} Texture;

class Atlas
{
public:
    Atlas();
    Atlas(std::string imageName, SDL_Renderer* renderer);
    int tileFromName(std::string tilename);
    int tileFromChar(char c);
    short tileX(int index);
    short tileY(int index);
    short tileW(int index);
    short tileH(int index);
    void sendImage(u8* pixels, int texID);
    const GLuint getTextureID();
private:
    void initCharTiles();
    void parseTiles(boost::filesystem::path fpath);
    GLuint textureID;
    std::vector<Texture> tiles;    //all textures in atlas
    std::map<char, int> charTiles; //glyph textures
    std::map<std::string, int> tileNames;
    
};

#endif

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
#include <iostream>
#include <string>
#include <ios>
#include <fstream>
#include <vector>
#include <map>
#include <stdexcept>
#include <boost/filesystem.hpp>
#include "Constants.h"
#include "GenTypes.h"
#include "DebugTools.h"
#include "SdlHeaders.h"

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
    Atlas(std::string imageName, SDL_Renderer* renderer);
    int tileFromName(std::string tilename);
    const Texture& textureFromName(std::string texname);
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

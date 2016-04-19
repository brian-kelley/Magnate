#ifndef __ATLAS_H__
#define __ATLAS_H__

#include "GenTypes.h"
#include "SdlHeaders.h"
#include <map>
#include <vector>
#include <boost/filesystem.hpp>

struct Texture
{
    std::string name;
    short x;
    short y;
    short width;
    short height;
};

namespace Atlas
{
    void init(std::string imageName);
    int tileFromName(std::string tilename);
    const Texture& textureFromName(std::string texname);
    const Texture& textureFromID(int id);
    int tileFromChar(char c);
    void sendImage(Color4* pixels, int texID);
    GLuint getTextureID();
    int getFontW();
    int getFontH();
    void initCharTiles();
    void parseTiles(boost::filesystem::path fpath);
    extern std::vector<Texture> tiles;    //all textures in atlas
    extern std::map<char, int> charTiles; //glyph textures
    extern std::map<std::string, int> tileNames;
    extern int fontW;
    extern int fontH;
    extern GLuint textureID;              //the texture ID# given by OpenGL
};

#endif

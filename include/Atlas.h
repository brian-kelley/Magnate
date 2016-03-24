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

class Atlas
{
public:
    static void init(std::string imageName);
    static int tileFromName(std::string tilename);
    static const Texture& textureFromName(std::string texname);
    static const Texture& textureFromID(int id);
    static int tileFromChar(char c);
    static void sendImage(Color4* pixels, int texID);
    static GLuint getTextureID();
    static int getFontW();
    static int getFontH();
private:
    static void initCharTiles();
    static void parseTiles(boost::filesystem::path fpath);
    static std::vector<Texture> tiles;    //all textures in atlas
    static std::map<char, int> charTiles; //glyph textures
    static std::map<std::string, int> tileNames;
    static int fontW;
    static int fontH;
    static GLuint textureID;              //the texture ID# given by OpenGL
};

#endif

//
//  Atlas.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/21/14294.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Atlas.h"

using namespace std;
using namespace boost::filesystem;
//Call this to initialize an atlas that already exists as assets/filename
Atlas::Atlas(string atlasName, SDL_Renderer* renderer)
{
    path imagePath = initial_path() / constants::BIN_TO_ROOT / "assets" / (atlasName + "_atlas.png");
    path tilePath = initial_path() / constants::BIN_TO_ROOT / "data" / (atlasName + "_tiles.txt");
    SDL_Surface* loadedSurface = IMG_Load(imagePath.string().c_str());
    size = loadedSurface->w;
    this->tex = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    parseTiles(tilePath);
    if(this->tex == nullptr)
    {
        cout << "Error: could not convert surface to tex." << endl;
        cout << SDL_GetError() << endl;
    }
    for(int i = 0; i < (int) tiles.size(); i++)
    {
        this->tileNames[tiles[i].name] = i;
    }
    SDL_FreeSurface(loadedSurface);
    initCharTiles();
}

Atlas::~Atlas()
{
    SDL_GL_UnbindTexture(this->tex);
    SDL_DestroyTexture(this->tex);
    this->tex = nullptr;
}

int Atlas::tileFromName(string tilename)
{
    return this->tileNames[tilename];
}

int Atlas::tileFromChar(char c)
{
    try
    {
        return this->charTiles[c];
    }
    catch (const std::out_of_range& oor)
    {
        cout << "Atlas couldn't find char '" << c << "'" << endl;
        cout << oor.what() << endl;
    }
    return 0;
}

void Atlas::bind()
{
    if(SDL_GL_BindTexture(this->tex, nullptr, nullptr) != 0)
    {
        cout << "Error binding atlas to GL context." << endl;
    }
}

void Atlas::initCharTiles()
{
    constants::FONTW = (int) (tiles[tileFromName("A")].width * float(size) + 0.5);
    constants::FONTH = (int) (tiles[tileFromName("A")].height * float(size) + 0.5);
    for(char lower = 'a'; lower <= 'z'; lower++)
    {
        this->charTiles[lower] = tileNames["_" + string({lower})];
    }
    for(char cap = 'A'; cap <= 'Z'; cap++)
    {
        this->charTiles[cap] = tileNames[string({cap})];
    }
    this->charTiles['~'] = tileNames["tilde"];
    this->charTiles['`'] = tileNames["accent"];
    this->charTiles['!'] = tileNames["exclamation"];
    this->charTiles['1'] = tileNames["one"];
    this->charTiles['@'] = tileNames["at"];
    this->charTiles['2'] = tileNames["two"];
    this->charTiles['#'] = tileNames["pound"];
    this->charTiles['3'] = tileNames["three"];
    this->charTiles['$'] = tileNames["dollar"];
    this->charTiles['4'] = tileNames["four"];
    this->charTiles['%'] = tileNames["percent"];
    this->charTiles['5'] = tileNames["five"];
    this->charTiles['^'] = tileNames["caret"];
    this->charTiles['6'] = tileNames["six"];
    this->charTiles['&'] = tileNames["ampersand"];
    this->charTiles['7'] = tileNames["seven"];
    this->charTiles['*'] = tileNames["asterisk"];
    this->charTiles['8'] = tileNames["eight"];
    this->charTiles['('] = tileNames["leftparen"];
    this->charTiles['9'] = tileNames["nine"];
    this->charTiles[')'] = tileNames["rightparen"];
    this->charTiles['0'] = tileNames["zero"];
    this->charTiles['_'] = tileNames["underscore"];
    this->charTiles['-'] = tileNames["dash"];
    this->charTiles['+'] = tileNames["plus"];
    this->charTiles['='] = tileNames["equals"];
    this->charTiles['{'] = tileNames["leftbrace"];
    this->charTiles['['] = tileNames["leftbracket"];
    this->charTiles['}'] = tileNames["rightbrace"];
    this->charTiles[']'] = tileNames["rightbracket"];
    this->charTiles['\\'] = tileNames["backslash"];
    this->charTiles['|'] = tileNames["bar"];
    this->charTiles[':'] = tileNames["colon"];
    this->charTiles[';'] = tileNames["semicolon"];
    this->charTiles['"'] = tileNames["quote"];
    this->charTiles['\''] = tileNames["apostrophe"];
    this->charTiles[','] = tileNames["comma"];
    this->charTiles['.'] = tileNames["period"];
    this->charTiles['<'] = tileNames["less"];
    this->charTiles['>'] = tileNames["greater"];
    this->charTiles['?'] = tileNames["question"];
    this->charTiles['/'] = tileNames["slash"];
}

void Atlas::parseTiles(path fpath)
{
    string line;
    ifstream tfile(fpath.string().c_str());
    unsigned long index;
    //pos is used to store search result to find whitespace in file
    if(tfile.is_open())
    {
        do
        {
            if(!tfile.eof())
            {
                getline(tfile, line);
                if(line != "" && line[0] != '#')
                {
                    tileData_t buffer;
                    index = 0;
                    buffer.name = line.substr(0, line.find(' '));
                    index = line.find(' ') + 1;
                    buffer.x = float(atoi(line.substr(index, line.find(' ', index)).c_str())) / size;
                    index = line.find(' ', index) + 1;
                    buffer.y = float(atoi(line.substr(index, line.find(' ', index)).c_str())) / size;
                    index = line.find(' ', index) + 1;
                    buffer.width = float(atoi(line.substr(index, line.find(' ', index)).c_str())) / size;
                    index = line.find(' ', index) + 1;
                    buffer.height = float(atoi(line.substr(index, line.find(' ', index)).c_str())) / size;
                    tiles.push_back(buffer);
                }
                else
                {
                    break;
                }
            }
        }
        while(line != "");
        tfile.close();
    }
    else
    {
        cout << "Error: could not load data/" + fpath.filename().string() << endl;
        cout << "Check if it exists and has file extension." << endl;
    }
}

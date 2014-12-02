//
//  Atlas.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/21/14294.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Atlas.h"

using namespace std;
//Call this to initialize an atlas that already exists as assets/filename
Atlas::Atlas(string imgDir, SDL_Renderer* renderer, int size)
{
    string file = imgDir + "_atlas.png";
    string tileData = file.substr(0, file.size() - 4) + "_tiles.dat";
    string path = DirManager::getPath() + "assets/" + file;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    //Automatically knows to generate the atlas if the file isn't found
    //Note: Assumes tile data file exists and is correct if the image exists.
    if(loadedSurface == NULL)
    {
        cout << "Atlas not found. Building a fresh one." << endl;
        buildAtlas(imgDir, renderer, size);
        loadedSurface = IMG_Load(path.c_str());
    }
    this->tex = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    this->width = loadedSurface->w;
    this->height = loadedSurface->h;
    if(this->tex == NULL)
    {
        cout << "Error: could not convert surface to tex." << endl;
        cout << SDL_GetError() << endl;
    }
    if(tileData.find(".dat") == string::npos)
    {
        tileData = tileData + ".dat";
    }
    this->tiles = DirManager::parseTiles(tileData, loadedSurface->w);
    for(int i = 0; i < (int) tiles.size(); i++)
    {
        this->tileNames[tiles[i].name] = i;
    }
    SDL_FreeSurface(loadedSurface);
    initCharTiles();
}

void Atlas::buildAtlas(string imgDir, SDL_Renderer* renderer, int size)
{
    string outputFile = DirManager::getPath() + "assets/" + imgDir + "_atlas.png";
    SDL_Surface* target = SDL_CreateRGBSurface(0, size, size, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
    SDL_Rect* wholeThing = new SDL_Rect;
    wholeThing->x = 0;
    wholeThing->y = 0;
    wholeThing->w = size;
    wholeThing->h = size;
    SDL_FillRect(target, wholeThing, 0x00000000);
    delete wholeThing;
    if(target == NULL)
    {
        cout << SDL_GetError() << endl;
        exit(1);
    }
    vector<string>* inputFiles = DirManager::exec("ls " + DirManager::getPath() + "assets/" + imgDir + "/*.png");
    ofstream tileFile((DirManager::getPath() + "data/" + imgDir + "_atlas_tiles.dat").c_str(), std::ios_base::out);
    vector<named_tex_t*> textures;
    string* path;
    for(int i = 0; i < (int) inputFiles->size(); i++)
    {
        path = &(*inputFiles)[i];
        textures.push_back(new named_tex_t);
        textures.back()->name = path->substr(path->rfind("/") + 1, path->find(".png") - path->rfind("/") - 1);
        textures.back()->surface = IMG_Load(path->c_str());
        if(textures.back()->surface == NULL)
        {
            cout << "Error: texture " << textures.back()->name << " is null." << endl;
        }
    }
    delete inputFiles;
    vector<SDL_Rect*> rects;
    rects.push_back(new SDL_Rect);
    rects[0]->x = 0;
    rects[0]->y = 0;
    rects[0]->w = size;
    rects[0]->h = size;
    sortForHeight(textures);
    //index of rect where tile will be placed
    int dest;
    while(!textures.empty())
    {
        dest = -1;
        for(int i = 0; i < (int) rects.size(); i++)
        {
            //Check for fit first
            if(rects[i]->w >= textures.back()->surface->w
               && rects[i]->h >= textures.back()->surface->h)
            {
                if(dest == -1)
                {
                    dest = i;
                }
                else if(rects[i]->h < rects[dest]->h)
                {
                    dest = i;
                }
            }
        }
        if(dest == -1)
        {
            cout << "Fatal error: texture " << textures.back()->name << " doesn't fit in atlas." << endl;
            exit(EXIT_FAILURE);
        }
        tileNames[textures.back()->name] = (int) this->tiles.size();
        tileFile << textures.back()->name << " " << rects[dest]->x << " " << rects[dest]->y << " " << textures.back()->surface->w << " " << textures.back()->surface->h << endl;
        int origW = rects[dest]->w;
        int origH = rects[dest]->h;
        SDL_BlitSurface(textures.back()->surface, NULL, target, rects[dest]);
        rects[dest]->w = origW;
        rects[dest]->h = origH;
        if(rects[dest]->w == textures.back()->surface->w
           && rects[dest]->h == textures.back()->surface->h)
        {
            delete rects[dest];
            rects[dest] = NULL;
            rects.erase(rects.begin() + dest);
        }
        else if(rects[dest]->w == textures.back()->surface->w)
        {
            rects[dest]->h -= textures.back()->surface->h;
            rects[dest]->y += textures.back()->surface->h;
        }
        else if(rects[dest]->h == textures.back()->surface->h)
        {
            rects[dest]->w -= textures.back()->surface->w;
            rects[dest]->x += textures.back()->surface->w;
        }
        else
        {
            SDL_Rect* newRect = new SDL_Rect;
            newRect->x = rects[dest]->x;
            newRect->y = rects[dest]->y + textures.back()->surface->h;
            newRect->w = rects[dest]->w;
            newRect->h = rects[dest]->h - textures.back()->surface->h;
            rects[dest]->x += textures.back()->surface->w;
            rects[dest]->h = textures.back()->surface->h;
            rects[dest]->w -= textures.back()->surface->w;
            rects.push_back(newRect);
        }
        SDL_FreeSurface(textures.back()->surface);
        textures.pop_back();
    }
    IMG_SavePNG(target, outputFile.c_str());
    while(rects.size() > 0)
    {
        delete rects.back();
        rects.pop_back();
    }
    this->tex = SDL_CreateTextureFromSurface(renderer, target);
    SDL_FreeSurface(target);
    target = NULL;
    tileFile.close();
}

Atlas::~Atlas()
{
    SDL_GL_UnbindTexture(this->tex);
    SDL_DestroyTexture(this->tex);
    this->tex = NULL;
}

void Atlas::sortForHeight(vector<named_tex_t*>& vec)
{
    int smallest;
    for(int i = 0; i < (int) vec.size() - 1; i++)
    {
        smallest = i;
        for(int j = 0; j < (int) vec.size(); j++)
        {
            if(vec[i]->surface->h < vec[smallest]->surface->h)
            {
                smallest = i;
            }
        }
        if(smallest != i)
        {
            named_tex_t temp = *vec[i];
            *vec[i] = *vec[smallest];
            *vec[smallest] = temp;
        }
    }
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
    if(SDL_GL_BindTexture(this->tex, NULL, NULL) != 0)
    {
        cout << "Error binding atlas to GL context." << endl;
        exit(EXIT_FAILURE);
    }
}

void Atlas::initCharTiles()
{
    constants::FONTW = (int) (this->tiles[this->tileFromName("A")].width * this->width + 0.5);
    constants::FONTH = (int) (this->tiles[this->tileFromName("A")].height * this->height + 0.5);
    for(char lower = 'a'; lower <= 'z'; lower++)
    {
        this->charTiles[lower] = tileNames["_" + lower];
    }
    for(char cap = 'A'; cap <= 'Z'; cap++)
    {
        this->charTiles[cap] = tileNames["" + cap];
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

#include "Atlas.h"

using namespace std;
using namespace boost::filesystem;

vector<Texture> Atlas::tiles;
map<char, int> Atlas::charTiles;
map<string, int> Atlas::tileNames;
int Atlas::fontW;
int Atlas::fontH;
GLuint Atlas::textureID;

//Call this to initialize an atlas that already exists as assets/filename
void Atlas::init(string atlasName, SDL_Renderer* renderer)
{
    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(IMG_INIT_PNG) & imgFlags))
    {
        cout << "Fatal error: View failed to initialize image loading." << endl;
        cout << "Not going to attempt loading main atlas." << endl;
        exit(EXIT_FAILURE);
    }
    path imagePath = initial_path() / FileIO::root / "assets" / (atlasName + "_atlas.png");
    path tilePath = initial_path() / FileIO::root / "data" / (atlasName + "_tiles.txt");
    SDL_Surface* loadedSurface = IMG_Load(imagePath.string().c_str());
    if(loadedSurface->w != loadedSurface->h)
        throw runtime_error("Atlas not square!");
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    #ifdef _WIN32
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, loadedSurface->w, loadedSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, loadedSurface->pixels);
    #elif __APPLE__
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, loadedSurface->w, loadedSurface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, loadedSurface->pixels);
    #endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    parseTiles(tilePath);
    for(int i = 0; i < (int) tiles.size(); i++)
    {
        tileNames[tiles[i].name] = i;
    }
    initCharTiles();
    SDL_FreeSurface(loadedSurface);
    GLERR
}

int Atlas::tileFromName(string tilename)
{
    DBASSERT(tileNames.find(tilename) != tileNames.end());
    return tileNames[tilename];
}

int Atlas::tileFromChar(char c)
{
    try
    {
        return charTiles[c];
    }
    catch (const std::out_of_range& oor)
    {
        cout << "Atlas couldn't find char '" << c << "'" << endl;
        cout << oor.what() << endl;
    }
    return 0;
}

void Atlas::initCharTiles()
{
    fontW = (int) (tiles[tileFromName("A")].width);
    fontH = (int) (tiles[tileFromName("A")].height);
    for(char lower = 'a'; lower <= 'z'; lower++)
    {
        charTiles[lower] = tileNames["_" + string({lower})];
    }
    for(char cap = 'A'; cap <= 'Z'; cap++)
    {
        charTiles[cap] = tileNames[string({cap})];
    }
    charTiles['~'] = tileNames["tilde"];
    charTiles['`'] = tileNames["accent"];
    charTiles['!'] = tileNames["exclamation"];
    charTiles['1'] = tileNames["one"];
    charTiles['@'] = tileNames["at"];
    charTiles['2'] = tileNames["two"];
    charTiles['#'] = tileNames["pound"];
    charTiles['3'] = tileNames["three"];
    charTiles['$'] = tileNames["dollar"];
    charTiles['4'] = tileNames["four"];
    charTiles['%'] = tileNames["percent"];
    charTiles['5'] = tileNames["five"];
    charTiles['^'] = tileNames["caret"];
    charTiles['6'] = tileNames["six"];
    charTiles['&'] = tileNames["ampersand"];
    charTiles['7'] = tileNames["seven"];
    charTiles['*'] = tileNames["asterisk"];
    charTiles['8'] = tileNames["eight"];
    charTiles['('] = tileNames["leftparen"];
    charTiles['9'] = tileNames["nine"];
    charTiles[')'] = tileNames["rightparen"];
    charTiles['0'] = tileNames["zero"];
    charTiles['_'] = tileNames["underscore"];
    charTiles['-'] = tileNames["dash"];
    charTiles['+'] = tileNames["plus"];
    charTiles['='] = tileNames["equals"];
    charTiles['{'] = tileNames["leftbrace"];
    charTiles['['] = tileNames["leftbracket"];
    charTiles['}'] = tileNames["rightbrace"];
    charTiles[']'] = tileNames["rightbracket"];
    charTiles['\\'] = tileNames["backslash"];
    charTiles['|'] = tileNames["bar"];
    charTiles[':'] = tileNames["colon"];
    charTiles[';'] = tileNames["semicolon"];
    charTiles['"'] = tileNames["quotation"];
    charTiles['\''] = tileNames["apostrophe"];
    charTiles[','] = tileNames["comma"];
    charTiles['.'] = tileNames["period"];
    charTiles['<'] = tileNames["less"];
    charTiles['>'] = tileNames["greater"];
    charTiles['?'] = tileNames["question"];
    charTiles['/'] = tileNames["slash"];
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
                    Texture buffer;
                    index = 0;
                    buffer.name = line.substr(0, line.find(' '));
                    index = line.find(' ') + 1;
                    buffer.x = atoi(line.substr(index, line.find(' ', index)).c_str());
                    index = line.find(' ', index) + 1;
                    buffer.y = atoi(line.substr(index, line.find(' ', index)).c_str());
                    index = line.find(' ', index) + 1;
                    buffer.width = atoi(line.substr(index, line.find(' ', index)).c_str());
                    index = line.find(' ', index) + 1;
                    buffer.height = atoi(line.substr(index, line.find(' ', index)).c_str());
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

void Atlas::sendImage(Color4* pixels, int texID)
{
    const auto& tex = textureFromID(texID);
    //OS X and Windows have different color orderings
#ifdef __APPLE__
    glTexSubImage2D(GL_TEXTURE_2D, 0, tex.x, tex.y, tex.width, tex.height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
#elif _WIN32
    glTexSubImage2D(GL_TEXTURE_2D, 0, destRect.x, destRect.y, destRect.width, destRect.height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
#endif
}

const Texture& Atlas::textureFromName(std::string texname)
{
    return tiles[tileFromName(texname)];
}

const Texture& Atlas::textureFromID(int id)
{
    return tiles[id];
}

int Atlas::getFontW()
{
    return fontW;
}

int Atlas::getFontH()
{
    return fontH;
}
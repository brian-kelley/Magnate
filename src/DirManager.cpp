//
//  DirManager.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/21/14294.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "DirManager.h"

using namespace std;

string DirManager::dirPath;

void DirManager::findPath()
{
    //Todo: actually get this from the exe location? (if possible)
#ifdef __APPLE__
    DirManager::dirPath = "/Users/Brian/Dropbox/Magnate/";
#elif __linux
    DirManager::dirPath = "/home/brian/Dropbox/Magnate";
#elif _WIN32

#endif
}

string DirManager::getPath()
{
    return DirManager::dirPath;
}

vector<DirManager::tileData_t> DirManager::parseTiles(string fname, float size)
{
    if(fname.find(".txt") == string::npos)
    {
        fname += ".txt";
    }
    vector<DirManager::tileData_t> tiles;
    string line;
    //theoretically this path should be the same for debug and release builds
    ifstream tfile(dirPath + "data/" + fname);
    unsigned long index;
    //pos is used to store search result to find whitespace in file
    DirManager::tileData_t buffer;
    if(tfile.is_open())
    {
        do
        {
            getNextLine(tfile, line);
            if(line != "")
            {
                index = 0;
                buffer.name = delim(index, line);
                buffer.x = ((float) delimInt(index, line)) / size;
                buffer.y = ((float) delimInt(index, line)) / size;
                buffer.width = ((float) delimInt(index, line)) / size;
                buffer.height = ((float) delimInt(index, line)) / size;
                tiles.push_back(buffer);
            }
            else
            {
                break;
            }
        }
        while(line != "");
        tfile.close();
    }
    else
    {
        cout << "Error: could not load data/" + fname << endl;
        cout << "Check if it exists and has file extension." << endl;
    }
    return tiles;
}
/*
Scene* DirManager::parseScene(string fname)
{
    Scene* out = new Scene();
    ifstream file(dirPath + "data/ui/" + fname);
    string data;
    if(file.is_open())
    {
        getNextLine(file, data);
        if(data != "")
        {
            if(data == "Button")
            {
                out->addButton(parseButton(file));
            }
            else if(data == "Label")
            {
                out->addLabel(parseLabel(file));
            }
            else if(data == "Field")
            {
                out->addField(parseField(file));
            }
        }
    }
    return out;
}

vector<Scene*>* DirManager::parseScenes()
{
    vector<Scene*>* vec = new vector<Scene*>;
    vector<string>* list = exec("ls -1 " + dirPath + "data/ui");
    for(int i = 0; i < (int) list->size(); i++)
    {
        vec->push_back(parseScene((*list)[i]));
    }
    return vec;
}
*/
string DirManager::delim(unsigned long& index, string& data)
{
    unsigned long e;
    if(index == data.length())
    {
        return "";
    }
    unsigned long b = index;
    e = data.find(' ', index);
    if(e == string::npos)
    {
        index = data.length();
        return data.substr(b, data.length() - b);
    }
    else
    {
        index = e + 1;
        return data.substr(b, e - b);
    }
}
//index will be updated with the start index of the next delimited string
int DirManager::delimInt(unsigned long& index, string& data)
{
    int out = 1;
    string stp = delim(index, data);
    if(stp != "")
    {
        try
        {
        	out = stoi(stp);
        }
        catch (const std::invalid_argument& invArg)
        {
            cout << "Error parsing int:" << invArg.what() << "\n";
            cout << "Outputting 1 instead.\n";
        }
    }
    else
    {
        cout << "Cannot parse int from null string.\n";
    }
    return out;
}
/*
Button* DirManager::parseButton(ifstream& file)
{
    string txt;
    getNextLine(file, txt);
    unsigned long index = 0;
    string data;
    getNextLine(file, data);
    int x = delimInt(index, data);
    int y = delimInt(index, data);
    int width = delimInt(index, data);
    int height = delimInt(index, data);
    return new Button(x, y, width, height, txt);
}

Label* DirManager::parseLabel(ifstream& file)
{
    string txt;
    getNextLine(file, txt);
    unsigned long index = 0;
    string data;
    getNextLine(file, data);
    int x = delimInt(index, data);
    int y = delimInt(index, data);
    int width = delimInt(index, data);
    int height = delimInt(index, data);
    return new Label(x, y, width, height, txt);
}

Field* DirManager::parseField(ifstream& file)
{
    string data;
    getNextLine(file, data);
    unsigned long index = 0;
    int x = delimInt(index, data);
    int y = delimInt(index, data);
    int width = delimInt(index, data);
    int height = delimInt(index, data);
    return new Field(x, y, width, height);
}
*/
vector<string>* DirManager::exec(string cmd)
{
    FILE* pipe = popen(cmd.c_str(), "r");
    if(!pipe)
    {
        cout << "Error: Could not open process." << endl;
        return nullptr;
    }
    else
    {
        char buffer[128];
        vector<string>* result = new vector<string>();
        while(!feof(pipe))
        {
            if(fgets(buffer, 200, pipe) != nullptr)
            {
                int i;
                for(i = 0; i < 200; i++)
                {
                    if(buffer[i] == '\n')
                    {
                        break;
                    }
                }
                result->push_back(string(buffer, buffer + i));
            }
        }
        pclose(pipe);
        return result;
    }
}

void DirManager::getNextLine(ifstream& file, string& data)
{
    data = "";
    while((data == "" && !(file.eof())))
    {
        getline(file, data);
        if(data.length() > 0 && data[0] == '#')
        {
            data = "";
        }
    }
}

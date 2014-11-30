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
    DirManager::dirPath = "/Users/Brian/Documents/MagIndev/MagIndev/";
}

string DirManager::getPath()
{
    return DirManager::dirPath;
}

vector<DirManager::tileData_t> DirManager::parseTiles(string fname, float size)
{
    if(fname.find(".dat") == string::npos)
    {
        fname += ".dat";
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
            getline(tfile, line);
            if(line[0] == '#' || line.length() < 8)
            {
                continue;
            }
            else
            {
                index = 0;
                buffer.name = delim(index, line);
                buffer.x = ((float) delimInt(index, line)) / size;
                buffer.y = ((float) delimInt(index, line)) / size;
                buffer.width = ((float) delimInt(index, line)) / size;
                buffer.height = ((float) delimInt(index, line)) / size;
                tiles.push_back(buffer);
            }
        }
        while(line.length() > 0);
        tfile.close();
    }
    else
    {
        cout << "Error: could not load data/" + fname << endl;
        cout << "Check if it exists and has file extension." << endl;
    }
    return tiles;
}

vector<Scene*>* DirManager::parseScenes(string path)
{
    //Put the vector of scenes* on the heap
    vector<Scene*>* vec = new vector<Scene*>;
    ifstream file(dirPath + "data/" + path);
    unsigned long index;
    string data;
    string type;
    Scene buffer;
    if(file.is_open())
    {
        do
        {
            getline(file, data);
            if(data[0] == '#' || data.length() < 2)
            {
                continue;
            }
            else
            {
                index = 0;
                type = data;
                //Begin parsing a scene here
                if(type == "Scene")
                {
                    buffer.clearAll();
                    while(data != "endScene")
                    {
                        getline(file, data);
                        type = data;
                        if(type == "Button")
                        {
                            buffer.addButton(parseButton(file));
                        }
                        else if(type == "Label")
                        {
                            buffer.addLabel(parseLabel(file));
                        }
                        else if(type == "Field")
                        {
                            buffer.addField(parseField(file));
                        }
                    }
                    vec->push_back(new Scene(buffer));
                }
            }
        }
        //run until EOF
        while(data.length() > 0);
    }
    else
    {
        cout << "Error: could not open " << path << endl;
    }
    return vec;
}

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
        catch (const std::invalid_argument& ia)
        {
            cout << "Error parsing int:" << ia.what() << "\n";
            cout << "Outputting 1 instead.\n";
        }
    }
    else
    {
        cout << "Cannot parse int from null string.\n";
    }
    return out;
}

Button* DirManager::parseButton(ifstream& file)
{
    string data;
    getline(file, data);
    string txt = data;
    getline(file, data);
    unsigned long index = 0;
    int x = delimInt(index, data);
    int y = delimInt(index, data);
    int width = delimInt(index, data);
    int height = delimInt(index, data);
    //Next call to getline(...) will be something else so leave it there
    return new Button(x, y, width, height, txt);
}

Label* DirManager::parseLabel(ifstream& file)
{
    string data;
    getline(file, data);
    string txt = data;
    unsigned long index = 0;
    getline(file, data);
    string strCenter = delim(index, data);
    bool centered = true;
    if(strCenter == "true")
    {
        centered = true;
    }
    else if(strCenter == "false")
    {
        centered = false;
    }
    else
    {
        cout << strCenter << " should be either 'true' or 'false'." << endl;
        cout << "Defaulting to false." << endl;
        centered = false;
    }
    unsigned long tempIndex = index;
    string def = delim(tempIndex, data);
    bool autoSized;
    if(def == "auto")
    {
        autoSized = true;
    }
    else
    {
        autoSized = false;
    }
    int x = delimInt(index, data);
    int y = delimInt(index, data);
    string det;
    int width = delimInt(index, data);
    int height = delimInt(index, data);
    string font = delim(index, data);
    return nullptr;
    //return new Label();
}

Field* DirManager::parseField(ifstream& file)
{
    return nullptr;
}

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
//
//  Constants.h
//  MagIndev
//
//  Created by Brian Kelley on 11/21/14325.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef MagIndev_Constants_h
#define MagIndev_Constants_h

#include <string>
#include <cmath>

//Structs used all over the place

struct floatRect_t
{
    float x;
    float y;
    float w;
    float h;
};

struct intRect_t
{
    int x;
    int y;
    int w;
    int h;
};

struct tileData_t
{
    std::string name;
    float x;
    float y;
    float width;
    float height;
};

struct isoPoint_t
{
    double i;
    double j;
    bool operator<(const isoPoint_t& param) const
    {
        if(i < param.i)
            return false;
        if(i > param.i)
            return true;
        if(j < param.j)
            return false;
        if(j > param.j)
            return true;
        return false;
    }

};

struct Point
{
    int x;
    int y;
    bool operator<(const Point& param) const
    {
        if(x < param.x)
            return false;
        if(x > param.x)
            return true;
        if(y < param.y)
            return false;
        if(y > param.y)
            return true;
        return false;
    }
};

//Pass pointer to anything in callback, unsafe but useful
//Callback function itself has to know what type the actual object has

typedef void (*callback_t) (void*);

enum GROUND : unsigned char
{
    WATER,
    PLAINS,
    FOREST,
    MOUNTAINS,
    DESERT,
    NUM_TYPES
};

enum VIEW_DIRECTION
{
    NORTH,
    SOUTH,
    EAST,
    WEST
};

//Type for terrain heightmap heights (uint8)

typedef unsigned char Height;

typedef struct
{
    GROUND g;
    Height height;
} tNode_t;

namespace constants
{
    extern int mouseX;
    extern int mouseY;
    extern int FONTW;
    extern int FONTH;
    extern int PAD;
    extern int BORDER_WIDTH;
    extern int BAR_WIDTH;
    extern int WINDOW_W;
    extern int WINDOW_H;
    extern float UI_BG_R;
    extern float UI_BG_G;
    extern float UI_BG_B;
    extern float UI_FG_R;
    extern float UI_FG_G;
    extern float UI_FG_B;
    extern float SHADE;
    extern float HMULT;
    extern std::string BIN_TO_ROOT;
    extern long int screenX;
    extern long int screenY;
    const double TERRAIN_TILE_SIZE = 0.25;
    const int WORLD_SIZE = 1025;
    const int CHUNK_SIZE = 61;
    const int WORLD_CHUNKS = WORLD_SIZE / CHUNK_SIZE + ((((WORLD_SIZE / CHUNK_SIZE) * CHUNK_SIZE) == WORLD_SIZE) ? 0 : 1);
    extern VIEW_DIRECTION viewDirection;
    const int PAN_SPEED = 15;   //pixels per frame
}

#endif
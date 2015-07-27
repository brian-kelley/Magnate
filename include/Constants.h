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

//Universal debug setting

#define MAGNATE_DEBUG

#ifdef MAGNATE_DEBUG
//Debug assert (segfault if condition not true)
#define DBASSERT(cond) if(!(cond)) throw runtime_error(#cond " not true!");
#define GLERR
//#define GLERR {auto errcode = glGetError(); if(errcode != GL_NO_ERROR) cout << errcode << endl; throw runtime_error("GL error!");}
#define PRINT(msg) {cout << "#msg" << endl;}
#else
#define DBASSERT
#define GLERR
#define PRINT
#endif

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
    extern double worldScale;
    const double maxScale = 2;
    const double minScale = 0.5;
    extern int FONTW;
    extern int FONTH;
    extern int PAD;
    extern int BORDER_WIDTH;
    extern int BAR_WIDTH;
    extern int WINDOW_W;
    extern int WINDOW_H;
    extern int ATLAS_SIZE;
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
    const int WORLD_SIZE = 2049;
    const int CHUNK_SIZE = 61;
    const int WORLD_CHUNKS = WORLD_SIZE / CHUNK_SIZE + ((((WORLD_SIZE / CHUNK_SIZE) * CHUNK_SIZE) == WORLD_SIZE) ? 0 : 1);
    extern VIEW_DIRECTION viewDirection;
    extern bool mouseDown;
    const int PAN_SPEED = 15;   //pixels per frame
    const int MINIMAP_SIZE = 256; //pixels in texture and on screen
    const int MINIMAP_BORDER = 15;
    const int BIT_DEPTH = 32;   //RGBA8888, probably won't change
    const double FOV = M_PI / 4; //TODO: make this configurable
    extern double camX;
    extern double camY;
    extern double camZ;
    extern double camAngle; //direction in xz, radians, east = 0, north = pi / 2
    const double camPitch = M_PI / 3;     //radians BELOW horizon (|elevation angle|)
    const int MAX_VBO_CHUNKS = 9; //depends on VRAM and desired max cam altitude - static, NOT adjusted at runtime.
    const int MAX_BUILDING_QUADS = 50000; //Initial max # of quads for non-terrain in VBO, extended at runtime if needed.
    const int MAX_GUI_QUADS = 2000;      //Max # of quads for GUI. Automatically extended at runtime if needed.
    const int MAX_GUI_LINES = 100;
}

#endif
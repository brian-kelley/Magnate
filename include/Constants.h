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
#include "GlmHeaders.h"
//Universal debug setting
#define MAGNATE_DEBUG

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
typedef unsigned char byte;
typedef short Height;

//Type for terrain heightmap heights (uint8)

namespace constants
{
    extern int mouseX;
    extern int mouseY;
    const double maxScale = 2;
    const double minScale = 0.5;
    extern int FONTW;
    extern int FONTH;      
    extern int PAD;
    const int BORDER_WIDTH = 5;
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
    extern std::string BIN_TO_ROOT;
    const double TERRAIN_TILE_SIZE = 1;
    const int WORLD_SIZE = 513;
    const int CHUNK_SIZE = 64;
    const int WORLD_CHUNKS = WORLD_SIZE / CHUNK_SIZE + ((((WORLD_SIZE / CHUNK_SIZE) * CHUNK_SIZE) == WORLD_SIZE) ? 0 : 1);
    extern bool mouseDown;
    const float PAN_SPEED = 0.02;   //world units/frame
    const float ZOOM_SPEED = 0.16;
    const float CAM_ROTATE_SPEED = M_PI / 60; //radian/s: full circle in 2s
    const float MAX_CAM_HEIGHT = 100; //depends on VBO_CHUNKS
    const int BIT_DEPTH = 32;   //RGBA8888, probably won't change
    const int VBO_CHUNKS = 25; //depends on VRAM and desired max cam altitude - static, NOT adjusted at runtime.
    const int MAX_BUILDING_QUADS = 50000; //Initial max # of quads for non-terrain in VBO, extended at runtime if needed.
    const int MAX_GUI_QUADS = 2000;      //Max # of quads for GUI. Automatically extended at runtime if needed.
    const int MAX_GUI_LINES = 100;
    const float TERRAIN_Y_SCALE = 0.05; //multiply by heightmap u8 values to get vertex y
}

#endif
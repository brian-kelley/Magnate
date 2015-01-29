//
//  Constants.h
//  MagIndev
//
//  Created by Brian Kelley on 11/21/14325.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef MagIndev_Constants_h
#define MagIndev_Constants_h

#include <cstdint>
#include <string>

//general action for buttons and fields, pass component
//ID so callee can identify component
typedef void (*callback_t) (int);

typedef struct
{
    float x;
    float y;
    float w;
    float h;
} floatRect_t;

typedef struct
{
    int x;
    int y;
    int w;
    int h;
} intRect_t;

typedef struct
{
    std::string name;
    float x;
    float y;
    float width;
    float height;
} tileData_t;

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
}

#endif
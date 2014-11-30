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

namespace constants
{
    extern int FONTW;
    extern int FONTH;
    extern int PAD;
    extern int BORDER_WIDTH;
    extern int DEFAULT_ATLAS_SIZE;
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
    enum STATES
    {
        TRACKING_MOUSE,
        TRACKING_KEYBOARD,
        UPDATING_VIEW,
        TERMINATING
    };
}

#endif
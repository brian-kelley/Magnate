//
//  Constants.cpp
//  MagIndev
//
//  Created by Brian Kelley on 11/21/14325.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "Constants.h"

using namespace std;
using namespace constants;

int constants::mouseX = 0;
int constants::mouseY = 0;
int constants::FONTW = -1;
int constants::FONTH = -1;
int constants::PAD = 5;
int constants::BORDER_WIDTH = 5;
int constants::BAR_WIDTH = 6;
int constants::WINDOW_W = 640;
int constants::WINDOW_H = 480;
int constants::ATLAS_SIZE; //initialized when main atlas is constructed
float constants::UI_BG_R = 0.5;
float constants::UI_BG_G = 0.5;
float constants::UI_BG_B = 0.5;
float constants::UI_FG_R = 0.75;
float constants::UI_FG_G = 0.75;
float constants::UI_FG_B = 0.75;
float constants::SHADE = 0.85;
float constants::HMULT = -80.0;
long int constants::screenX = 10000;
long int constants::screenY = 0;
VIEW_DIRECTION constants::viewDirection = NORTH;
double constants::worldScale = 1;
bool constants::mouseDown = false;
double constants::camX = 0;
double constants::camY = 20;
double constants::camZ = 20;
double constants::camAngle = 0;

#ifdef __APPLE__
string constants::BIN_TO_ROOT = "../..";
//Don't know yet exactly what linux/win values will be
#elif __linux
string constants::BIN_TO_ROOT = "../..";
#elif _WIN32
string constants::BIN_TO_ROOT = "..";
#endif
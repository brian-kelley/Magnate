//
//  Coord.h
//  Magnate
//
//  Created by Brian Kelley on 10/15/14288.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __Magnate__Coord__
#define __Magnate__Coord__

#include "Constants.h"
#include <SDL2/SDL.h>

namespace coord
{
    extern Point project3DPoint(double i, double j, double h);
    extern int ix(double i, double j);
    extern int jy(double i, double j);
    extern double xi(int x, int y);
    extern double yj(int x, int y);
    extern bool rectInside(intRect_t* small, intRect_t* big);
    //Base tile height (px)
    const int ISO_WIDTH = 64;
    const int ISO_LENGTH = ISO_WIDTH * 2;
    //Tiles can be 128 long, 64 high, and 1 unit of height in 3D space translates to this height (px) of screen space
    //Here the height comes out to 80, which looks pretty good
    const int ISO_HEIGHT = ISO_WIDTH * 5 / 4;

    /* sideways diagram of flat floor tile:
      /\      ]-|
     /  \       |           ^ Left on screen
    /    \      |- Length   ------> Upwards on screen
    \    /      |           v Right on screen
     \  /       |
      \/      ]-|
    /      \
   /        \
  v          v (+j direction)
 (+i direction)
   |-----|
      |
    Width

    */
};
#endif

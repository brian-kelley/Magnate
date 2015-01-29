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

int ix(double i, double j);
int jy(double i, double j);
double xi(int x, int y);
double yj(int x, int y);
bool rectInside(intRect_t& small, intRect_t& big);

#endif
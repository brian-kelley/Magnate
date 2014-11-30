//
//  Coord.cpp
//  Magnate
//
//  Created by Brian Kelley on 10/15/14288.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

int ix(double i, double j)
{
    return (int) (64 * i + 64 * j);
}

int jy(double i, double j)
{
    return (int) (32 * j - 32 * i);
}

double xi(int x, int y)
{
    return x / 128.0 - y / 64.0;
}

double yj(int x, int y)
{
    return x / 128.0 + y / 64.0;
}

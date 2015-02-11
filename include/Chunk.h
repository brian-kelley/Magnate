//
//  Chunk.h
//  MagIndev
//
//  Created by Brian Kelley on 10/22/14295.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __MagIndev__Chunk__
#define __MagIndev__Chunk__

#include <stdio.h>

class Chunk
{
public:
    Chunk(int, int);
    ~Chunk();
private:
    int x;
    int y;
};

#endif /* defined(__MagIndev__Chunk__) */

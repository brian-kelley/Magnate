//
//  Model.h
//  Magnate
//
//  Created by Brian Kelley on 10/15/14288.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __Magnate__Model__
#define __Magnate__Model__

#include <iostream>
#include <fstream>
#include "World.h"
#include "Chunk.h"
#include "Cuboid.h"
#include "Constants.h"

class Model
{
public:
    Model();
    ~Model();
    void update();
private:
    World* currentWorld;
};

#endif

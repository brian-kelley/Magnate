//
//  View.h
//  MagIndev
//
//  Created by Brian Kelley on 10/20/14293.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#ifndef __MagIndev__View__
#define __MagIndev__View__

#include <iostream>
#include <vector>
#include "GUI.h"
#include "UIRenderer.h"
#include "Atlas.h"
#include "Building.h"
#include "Coord.h"
#include "Renderer.h"
#include "TexManager.h"
#include "SdlHeaders.h"

class View
{
public:
    View();
    ~View();
private:
    Atlas atlas;
};

#endif
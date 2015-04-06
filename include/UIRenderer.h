//
//  UIRenderer.h
//  Magnate
//
//  Created by Brian Kelley on 3/5/1564.
//
//

#ifndef Magnate_UIRenderer_h
#define Magnate_UIRenderer_h

#include <gl.h>
#include <iostream>
#include <string>
#include <vector>
#include "RenderRoutines.h"
#include "Component.h"
#include "Constants.h"
#include "Button.h"
#include "Label.h"
#include "ScrollBlock.h"
#include "MultiSelect.h"
#include "Field.h"
#include "Scene.h"
#include "Renderer.h"

namespace UIRenderer
{
    extern void drawComponent(Component& c);
    extern void drawScene(Scene& s);
    extern void drawButton(Button& b);
    extern void drawLabel(Label& l);
    extern void drawField(Field& f);
    extern void drawScrollBlock(ScrollBlock& sb);
    extern void drawMultiSelect(MultiSelect& ms);
};

#endif
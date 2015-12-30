#ifndef __UIRENDERER_H__
#define __UIRENDERER_H__

#include "gl.h"
#include <iostream>
#include <string>
#include <vector>
#include "RenderRoutines.h"
#include "Component.h"
#include "Constants.h"
#include "Button.h"
#include "Label.h"
#include "ScrollBlock.h"
#include "Draggable.h"
#include "MultiSelect.h"
#include "Field.h"
#include "Scene.h"
#include "Renderer.h"
#include "Building.h"

namespace UIRenderer
{
    extern void drawComponent(Component& c);
    extern void drawScene(Scene& s);
    extern void drawButton(Button& b);
    extern void drawLabel(Label& l);
    extern void drawField(Field& f);
    extern void drawScrollBlock(ScrollBlock& sb);
    extern void drawMultiSelect(MultiSelect& ms);
    extern void drawDraggable(Draggable& d);
};

#endif
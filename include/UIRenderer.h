#ifndef __UIRENDERER_H__
#define __UIRENDERER_H__

#include <iostream>
#include <string>
#include <vector>
#include "Component.h"
#include "Button.h"
#include "Label.h"
#include "ScrollBlock.h"
#include "Draggable.h"
#include "MultiSelect.h"
#include "Field.h"
#include "Scene.h"
#include "Building.h"
#include "ImmediateDraw.h"

class UIRenderer
{
public:
    void drawComponent(Component& c);
    void drawScene(Scene& s);
    void drawButton(Button& b);
    void drawLabel(Label& l);
    void drawField(Field& f);
    void drawScrollBlock(ScrollBlock& sb);
    void drawMultiSelect(MultiSelect& ms);
    void drawDraggable(Draggable& d);
    static const int GUI_QUAD_PRELOAD = 400; //How many vertices to reserve() in
    static const int GUI_LINE_PRELOAD = 10;  //GUI quad and line attrib vectors
    static const int MAX_GUI_LINES = 30;
    static const int MAX_GUI_QUADS = 200;
};

#endif
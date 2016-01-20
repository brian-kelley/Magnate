#ifndef __UIRENDERER_H__
#define __UIRENDERER_H__

#include <iostream>
#include <string>
#include <vector>
#include "Atlas.h"
#include "GUI.h"
#include "Component.h"
#include "Button.h"
#include "Label.h"
#include "ScrollBlock.h"
#include "Draggable.h"
#include "MultiSelect.h"
#include "Field.h"
#include "Scene.h"
#include "Minimap.h"
#include "ImmediateDraw.h"
#include "GlmHeaders.h"
#include "VBO.h"
#include "Coord.h"
#include "Input.h"

class UIRenderer
{
public:
    UIRenderer();
    void drawComponent(Component* c);
    void draw();         //do GL draw call for all visible components
private:
    void drawScene(Scene* s);
    void drawButton(Button* b);
    void drawLabel(Label* l);
    void drawField(Field* f);
    void drawScrollBlock(ScrollBlock* sb);
    void drawMultiSelect(MultiSelect* ms);
    void drawDraggable(Draggable* d);
    void drawMinimap(Minimap* mm);
    const int QUAD_INIT;
    const int LINE_INIT;
    ImmediateDraw imm;      //handles VBO for easy drawing
    //Aesthetic constants
    const Color4 fg;
    const Color4 bg;
    const int BORDER_WIDTH;
    const int PAD;
    const int BAR_WIDTH;
    const float SHADE;
};

#endif
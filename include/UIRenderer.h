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
#include "Camera.h"

namespace UIRenderer
{
    void drawComponent(Component* c);
    void draw();         //do GL draw call for all visible components
    void drawScene(Scene* s);
    void drawButton(Button* b);
    void drawLabel(Label* l);
    void drawField(Field* f);
    void drawScrollBlock(ScrollBlock* sb);
    void drawMultiSelect(MultiSelect* ms);
    void drawDraggable(Draggable* d);
    void drawMinimap(Minimap* mm);
    //Aesthetic constants
    extern const Color4 fg;
    extern const Color4 bg;
    extern const int BORDER_WIDTH;
    extern const int PAD;
    extern const int BAR_WIDTH;
    extern const float SHADE;
}

#endif

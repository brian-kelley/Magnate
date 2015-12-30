#ifndef __GUI_H__
#define __GUI_H__

#include "Component.h"
#include "Scene.h"
#include "Button.h"
#include "Label.h"
#include "Field.h"
#include "MultiSelect.h"
#include "ScrollBlock.h"

namespace GUI
{
    void init(Scene* initial);
    void processResize(int w, int h);
    void transition(Scene* next);
    extern Scene* current;
}

#endif
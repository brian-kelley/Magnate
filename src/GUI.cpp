#include "GUI.h"

Scene* GUI::current;

void GUI::init(Scene *initial)
{
    current = initial;
}

void GUI::processResize(int w, int h)
{
    current->processResize();
}

void GUI::transition(Scene *next)
{
    current = next;
}
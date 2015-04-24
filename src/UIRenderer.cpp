//
//  UIRenderer.cpp
//  Magnate
//
//  Created by Brian Kelley on 3/5/1564.
//
//

#include "UIRenderer.h"
#include <stdio.h>

using namespace std;
using namespace constants;
using namespace RenderRoutines;
using namespace Renderer;

void UIRenderer::drawComponent(Component& c)
{
    intRect_t& drect = c.getDrawRect();
    if(c.getParent() == nullptr)
    {
        glDisable(GL_SCISSOR_TEST);
    }
    else
    {
        intRect_t& parentDraw = c.getParent()->getDrawRect();
        glScissor(parentDraw.x, WINDOW_H - parentDraw.y - parentDraw.h, parentDraw.w, parentDraw.h);
        glEnable(GL_SCISSOR_TEST);
    }
    switch(c.getType())
    {
        case SCROLLBLOCK:
            drawScrollBlock((ScrollBlock&) c);
            break;
        case BUTTON:
            drawButton((Button&) c);
            break;
        case LABEL:
            drawLabel((Label&) c);
            break;
        case FIELD:
            drawField((Field&) c);
            break;
        case MULTISELECT:
            drawMultiSelect((MultiSelect&) c);
            break;
        case DRAGGABLE:
            
            break;
        case SCENE:
            break;
    }
    //now go in one level and draw child components of c
    for(Component* child : c.getChildren())
    {
        intRect_t& crect = child->getDrawRect();
        if(crect.x <= drect.x + drect.w && crect.x + crect.w >= drect.x
           && crect.y <= drect.y + drect.h && crect.y + crect.h >= drect.y)
        {
            drawComponent(*child);
        }
    }
    glDisable(GL_SCISSOR_TEST);
}

void UIRenderer::drawMultiSelect(MultiSelect& ms)
{
    if(ms.getNumOptions() > 0)
    {
        vector<string>& list = ms.getOptions();
        intRect_t& msrect = ms.getDrawRect();
        disableTexture();
        color3f(UI_BG_R, UI_BG_G, UI_BG_B);
        vertex2i(msrect.x, msrect.y);
        vertex2i(msrect.x + msrect.w, msrect.y);
        vertex2i(msrect.x + msrect.w, msrect.y + msrect.h);
        vertex2i(msrect.x, msrect.y + msrect.h);
        if(ms.getSelection() != -1)
        {
            color3f(UI_FG_R, UI_FG_G, UI_FG_B);
            int optY = msrect.y + ms.getSelection() * ms.getOptHeight();
            vertex2i(msrect.x, optY);
            vertex2i(msrect.x + msrect.w, optY);
            vertex2i(msrect.x + msrect.w, optY + PAD);
            vertex2i(msrect.x, optY + PAD);
            vertex2i(msrect.x, optY + PAD);
            vertex2i(msrect.x + PAD, optY + PAD);
            vertex2i(msrect.x + PAD, optY + ms.getOptHeight() - PAD);
            vertex2i(msrect.x, optY + ms.getOptHeight() - PAD);
            vertex2i(msrect.x + msrect.w - PAD, optY + PAD);
            vertex2i(msrect.x + msrect.w, optY + PAD);
            vertex2i(msrect.x + msrect.w, optY + ms.getOptHeight() - PAD);
            vertex2i(msrect.x + msrect.w - PAD, optY + ms.getOptHeight() - PAD);
            vertex2i(msrect.x, optY + ms.getOptHeight() - PAD);
            vertex2i(msrect.x + msrect.w, optY + ms.getOptHeight() - PAD);
            vertex2i(msrect.x + msrect.w, optY + ms.getOptHeight());
            vertex2i(msrect.x, optY + ms.getOptHeight());
        }
        for(int i = 0; i < int(list.size()); i++)
        {
            drawString(list[i], PAD + msrect.x, PAD + msrect.y + i * ms.getOptHeight(), ms.getFontScale(), UI_FG_R, UI_FG_G, UI_FG_B);
        }
    }
}

void UIRenderer::drawLabel(Label& l)
{
    intRect_t& lrect = l.getDrawRect();
    drawString(l.getText(), lrect.x + l.getTextLoc().x, lrect.y + l.getTextLoc().y, l.getFontScale(), UI_FG_R * SHADE * SHADE, UI_FG_G * SHADE * SHADE, UI_FG_B * SHADE * SHADE);
}

void UIRenderer::drawField(Field& f)
{
    intRect_t& curRect = f.getDrawRect();
    disableTexture();
    color3f(UI_FG_R, UI_FG_G, UI_FG_B);
    vertex2i(curRect.x, curRect.y);
    vertex2i(curRect.x + curRect.w, curRect.y);
    vertex2i(curRect.x + curRect.w, curRect.y + curRect.h);
    vertex2i(curRect.x, curRect.y + curRect.h);
    color3f(UI_BG_R, UI_BG_G, UI_BG_B);
    vertex2i(curRect.x + 1, curRect.y + 1);
    vertex2i(curRect.x + curRect.w - 1, curRect.y + 1);
    vertex2i(curRect.x + 1, curRect.y + curRect.h - 1);
    drawString(f.getText(), curRect.x + f.getTextLoc().x, curRect.y + f.getTextLoc().y, f.getFontScale(), UI_FG_R, UI_FG_G, UI_FG_B);
}

void UIRenderer::drawButton(Button& b)
{
    float colorMult;
    if(!b.isMouseOver())
    {
        colorMult = 1.0f;
    }
    else
    {
        colorMult = 1.0f / SHADE;
    }
    intRect_t rect = b.getDrawRect();         //initalize copy, don't modify
    disableTexture();
    color3f(UI_BG_R, UI_BG_G, UI_BG_B);
    vertex2i(rect.x + BORDER_WIDTH, rect.y + BORDER_WIDTH);
    vertex2i(rect.x + rect.w - BORDER_WIDTH, rect.y + BORDER_WIDTH);
    vertex2i(rect.x + rect.w - BORDER_WIDTH, rect.y + rect.h - BORDER_WIDTH);
    vertex2i(rect.x + BORDER_WIDTH, rect.y + rect.h - BORDER_WIDTH);
    color3f(UI_FG_R * colorMult, UI_FG_G * colorMult, UI_FG_B * colorMult);
    vertex2i(rect.x, rect.y);
    vertex2i(rect.x + rect.w, rect.y);
    vertex2i(rect.x + rect.w - BORDER_WIDTH, rect.y + BORDER_WIDTH);
    vertex2i(rect.x + BORDER_WIDTH, rect.y + BORDER_WIDTH);
    vertex2i(rect.x, rect.y);
    vertex2i(rect.x + BORDER_WIDTH, rect.y + BORDER_WIDTH);
    vertex2i(rect.x + BORDER_WIDTH, rect.y + rect.h - BORDER_WIDTH);
    vertex2i(rect.x, rect.y + rect.h);
    color3f(UI_FG_R * colorMult * SHADE, UI_FG_G * colorMult * SHADE, UI_FG_B * colorMult * SHADE);
    vertex2i(rect.x + rect.w, rect.y);
    vertex2i(rect.x + rect.w, rect.y + rect.h);
    vertex2i(rect.x + rect.w - BORDER_WIDTH, rect.y + rect.h - BORDER_WIDTH);
    vertex2i(rect.x + rect.w - BORDER_WIDTH, rect.y + BORDER_WIDTH);
    vertex2i(rect.x + BORDER_WIDTH, rect.y + rect.h - BORDER_WIDTH);
    vertex2i(rect.x + rect.w - BORDER_WIDTH, rect.y + rect.h - BORDER_WIDTH);
    vertex2i(rect.x + rect.w, rect.y + rect.h);
    vertex2i(rect.x, rect.y + rect.h);
    drawString(b.getText(), b.getTextLoc().x + rect.x, b.getTextLoc().y + rect.y, b.getFontScale(), UI_FG_R, UI_FG_G, UI_FG_B);
}

void UIRenderer::drawScrollBlock(ScrollBlock& sb)
{
    color3f(UI_BG_R * SHADE, UI_BG_G * SHADE, UI_BG_B * SHADE);
    disableTexture();
    intRect_t& sbrect = sb.getDrawRect();
    vertex2i(sbrect.x, sbrect.y);
    vertex2i(sbrect.x + sbrect.w, sbrect.y);
    vertex2i(sbrect.x + sbrect.w, sbrect.y + sbrect.h);
    vertex2i(sbrect.x, sbrect.y + sbrect.h);
    //For some reason glScissor wants (x, y) to be lower-left corner
    if(sb.hasBar())
    {
        intRect_t bar = sb.getBarRect();
        color3f(UI_FG_R, UI_FG_G, UI_FG_B);
        vertex2i(bar.x, bar.y);
        vertex2i(bar.x + bar.w, bar.y);
        vertex2i(bar.x + bar.w, bar.y + bar.h);
        vertex2i(bar.x, bar.y + bar.h);
    }
}

void UIRenderer::drawDraggable(Draggable& d)
{
    //First draw the image at base position, this will always happen,
    //then draw the dragging image if d is active
    switch(d.getImageType())
    {
        case IMAGE:
            //Simply blit the image num as a texture ID
            blit(d.getImageNum(), d.getDrawRect().x, d.getDrawRect().y);
            if(d.isActive())
            {
                blit(d.getImageNum(), d.getDragPos().x, d.getDragPos().y);
            }
            break;
        case BUILDING:
            
            break;
    }
}
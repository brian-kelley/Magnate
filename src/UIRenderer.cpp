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
        glDisable(GL_TEXTURE_2D);
        glColor3f(UI_BG_R, UI_BG_G, UI_BG_B);
        glBegin(GL_QUADS);
        glVertex2i(msrect.x, msrect.y);
        glVertex2i(msrect.x + msrect.w, msrect.y);
        glVertex2i(msrect.x + msrect.w, msrect.y + msrect.h);
        glVertex2i(msrect.x, msrect.y + msrect.h);
        glEnd();
        if(ms.getSelection() != -1)
        {
            glColor3f(UI_FG_R, UI_FG_G, UI_FG_B);
            int optY = msrect.y + ms.getSelection() * ms.getOptHeight();
            glBegin(GL_QUADS);
            glVertex2i(msrect.x, optY);
            glVertex2i(msrect.x + msrect.w, optY);
            glVertex2i(msrect.x + msrect.w, optY + PAD);
            glVertex2i(msrect.x, optY + PAD);
            glEnd();
            glBegin(GL_QUADS);
            glVertex2i(msrect.x, optY + PAD);
            glVertex2i(msrect.x + PAD, optY + PAD);
            glVertex2i(msrect.x + PAD, optY + ms.getOptHeight() - PAD);
            glVertex2i(msrect.x, optY + ms.getOptHeight() - PAD);
            glEnd();
            glBegin(GL_QUADS);
            glVertex2i(msrect.x + msrect.w - PAD, optY + PAD);
            glVertex2i(msrect.x + msrect.w, optY + PAD);
            glVertex2i(msrect.x + msrect.w, optY + ms.getOptHeight() - PAD);
            glVertex2i(msrect.x + msrect.w - PAD, optY + ms.getOptHeight() - PAD);
            glEnd();
            glBegin(GL_QUADS);
            glVertex2i(msrect.x, optY + ms.getOptHeight() - PAD);
            glVertex2i(msrect.x + msrect.w, optY + ms.getOptHeight() - PAD);
            glVertex2i(msrect.x + msrect.w, optY + ms.getOptHeight());
            glVertex2i(msrect.x, optY + ms.getOptHeight());
            glEnd();
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
    glDisable(GL_TEXTURE_2D);
    glColor3f(UI_BG_R, UI_BG_G, UI_BG_B);
    glBegin(GL_QUADS);
    glVertex2i(curRect.x, curRect.y);
    glVertex2i(curRect.x + curRect.w, curRect.y);
    glVertex2i(curRect.x + curRect.w, curRect.y + curRect.h);
    glVertex2i(curRect.x, curRect.y + curRect.h);
    glEnd();
    glColor3f(constants::UI_FG_R, constants::UI_FG_G, constants::UI_FG_B);
    glBegin(GL_LINES);
    glVertex2i(curRect.x, curRect.y);
    glVertex2i(curRect.x + curRect.w, curRect.y);
    glVertex2i(curRect.x + curRect.w, curRect.y);
    glVertex2i(curRect.x + curRect.w, curRect.y + curRect.h);
    glVertex2i(curRect.x + curRect.w, curRect.y + curRect.h);
    glVertex2i(curRect.x, curRect.y + curRect.h);
    glVertex2i(curRect.x, curRect.y);
    glVertex2i(curRect.x, curRect.y + curRect.h);
    glEnd();
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
    glDisable(GL_TEXTURE_2D);
    glColor3f(UI_BG_R, UI_BG_G, UI_BG_B);
    glBegin(GL_QUADS);
    glVertex2i(rect.x + BORDER_WIDTH, rect.y + BORDER_WIDTH);
    glVertex2i(rect.x + rect.w - BORDER_WIDTH, rect.y + BORDER_WIDTH);
    glVertex2i(rect.x + rect.w - BORDER_WIDTH, rect.y + rect.h - BORDER_WIDTH);
    glVertex2i(rect.x + BORDER_WIDTH, rect.y + rect.h - BORDER_WIDTH);
    glEnd();
    glColor3f(UI_FG_R * colorMult, UI_FG_G * colorMult, UI_FG_B * colorMult);
    glBegin(GL_QUADS);
    glVertex2i(rect.x, rect.y);
    glVertex2i(rect.x + rect.w, rect.y);
    glVertex2i(rect.x + rect.w - BORDER_WIDTH, rect.y + BORDER_WIDTH);
    glVertex2i(rect.x + BORDER_WIDTH, rect.y + BORDER_WIDTH);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2i(rect.x, rect.y);
    glVertex2i(rect.x + BORDER_WIDTH, rect.y + BORDER_WIDTH);
    glVertex2i(rect.x + BORDER_WIDTH, rect.y + rect.h - BORDER_WIDTH);
    glVertex2i(rect.x, rect.y + rect.h);
    glEnd();
    glColor3f(UI_FG_R * colorMult * SHADE, UI_FG_G * colorMult * SHADE, UI_FG_B * colorMult * SHADE);
    glBegin(GL_QUADS);
    glVertex2i(rect.x + rect.w, rect.y);
    glVertex2i(rect.x + rect.w, rect.y + rect.h);
    glVertex2i(rect.x + rect.w - BORDER_WIDTH, rect.y + rect.h - BORDER_WIDTH);
    glVertex2i(rect.x + rect.w - BORDER_WIDTH, rect.y + BORDER_WIDTH);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2i(rect.x + BORDER_WIDTH, rect.y + rect.h - BORDER_WIDTH);
    glVertex2i(rect.x + rect.w - BORDER_WIDTH, rect.y + rect.h - BORDER_WIDTH);
    glVertex2i(rect.x + rect.w, rect.y + rect.h);
    glVertex2i(rect.x, rect.y + rect.h);
    glEnd();
    drawString(b.getText(), b.getTextLoc().x + rect.x, b.getTextLoc().y + rect.y, b.getFontScale(), UI_FG_R, UI_FG_G, UI_FG_B);
}

void UIRenderer::drawScrollBlock(ScrollBlock& sb)
{
    glColor4f(UI_BG_R * SHADE, UI_BG_G * SHADE, UI_BG_B * SHADE, 1);
    glDisable(GL_TEXTURE_2D);
    intRect_t& sbrect = sb.getDrawRect();
    glBegin(GL_QUADS);
    glVertex2i(sbrect.x, sbrect.y);
    glVertex2i(sbrect.x + sbrect.w, sbrect.y);
    glVertex2i(sbrect.x + sbrect.w, sbrect.y + sbrect.h);
    glVertex2i(sbrect.x, sbrect.y + sbrect.h);
    glEnd();
    //For some reason glScissor wants (x, y) to be lower-left corner
    if(sb.hasBar())
    {
        intRect_t bar = sb.getBarRect();
        
        glColor3f(UI_FG_R, UI_FG_G, UI_FG_B);
        glBegin(GL_QUADS);
        glVertex2i(bar.x, bar.y);
        glVertex2i(bar.x + bar.w, bar.y);
        glVertex2i(bar.x + bar.w, bar.y + bar.h);
        glVertex2i(bar.x, bar.y + bar.h);
        glEnd();
    }
}

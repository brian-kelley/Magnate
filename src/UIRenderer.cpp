#include "UIRenderer.h"

using namespace std;

UIRenderer::UIRenderer() :
QUAD_INIT(1000),
LINE_INIT(100),
imm(QUAD_INIT, LINE_INIT),
fg(190, 190, 190, 255),
bg(120, 120, 120, 255),
BORDER_WIDTH(5),
PAD(5),
BAR_WIDTH(6),
SHADE(0.85)
{
    imm = ImmediateDraw(QUAD_INIT, LINE_INIT);
}

void UIRenderer::draw()
{
    imm.draw();
}

void UIRenderer::drawComponent(Component *c)
{
    switch(c->getType())
    {
        case CompType::scene:
            drawScene((Scene*) c);
            break;
        case CompType::button:
            drawButton((Button*) c);
            break;
        case CompType::label:
            drawLabel((Label*) c);
            break;
        case CompType::field:
            drawField((Field*) c);
            break;
        case CompType::multiSelect:
            drawMultiSelect((MultiSelect*) c);
            break;
        case CompType::scrollBlock:
            drawScrollBlock((ScrollBlock*) c);
            break;
        case CompType::draggable:
            drawDraggable((Draggable*) c);
            break;
        case CompType::minimap:
            drawMinimap((Minimap*) c);
            break;
    }
}

void UIRenderer::drawScene(Scene *s)
{
    for(auto c : s->getChildren())
        drawComponent(c);
}

void UIRenderer::drawMultiSelect(MultiSelect* ms)
{
    if(ms->getNumOptions() > 0)
    {
        vector<string>& list = ms->getOptions();
        auto rect = ms->getScreenRect();
        imm.disableTextures();
        imm.color3f(bg.r, bg.g, bg.b);
        imm.vertex2i(rect.x, rect.y);
        imm.vertex2i(rect.x + rect.w, rect.y);
        imm.vertex2i(rect.x + rect.w, rect.y + rect.h);
        imm.vertex2i(rect.x, rect.y + rect.h);
        if(ms->getSelection() != -1)
        {
            imm.color3f(fg.r, fg.g, fg.b);
            int optY = rect.y + ms->getSelection() * ms->getOptHeight();
            imm.vertex2i(rect.x, optY);
            imm.vertex2i(rect.x + rect.w, optY);
            imm.vertex2i(rect.x + rect.w, optY + PAD);
            imm.vertex2i(rect.x, optY + PAD);
            imm.vertex2i(rect.x, optY + PAD);
            imm.vertex2i(rect.x + PAD, optY + PAD);
            imm.vertex2i(rect.x + PAD, optY + ms->getOptHeight() - PAD);
            imm.vertex2i(rect.x, optY + ms->getOptHeight() - PAD);
            imm.vertex2i(rect.x + rect.w - PAD, optY + PAD);
            imm.vertex2i(rect.x + rect.w, optY + PAD);
            imm.vertex2i(rect.x + rect.w, optY + ms->getOptHeight() - PAD);
            imm.vertex2i(rect.x + rect.w - PAD, optY + ms->getOptHeight() - PAD);
            imm.vertex2i(rect.x, optY + ms->getOptHeight() - PAD);
            imm.vertex2i(rect.x + rect.w, optY + ms->getOptHeight() - PAD);
            imm.vertex2i(rect.x + rect.w, optY + ms->getOptHeight());
            imm.vertex2i(rect.x, optY + ms->getOptHeight());
        }
        for(int i = 0; i < int(list.size()); i++)
        {
            //imm.drawString(list[i], PAD + rect.x, PAD + rect.y + i * ms->getOptHeight(), ms->getFontScale(), fg.r, fg.g, fg.b);
        }
    }
}

void UIRenderer::drawLabel(Label* l)
{
    auto lrect = l->getScreenRect();
    //drawString(l.getText(), lrect.x + l.getTextLoc().x, lrect.y + l.getTextLoc().y, l.getFontScale(), UI_FG_R * SHADE * SHADE, UI_FG_G * SHADE * SHADE, UI_FG_B * SHADE * SHADE);
}

void UIRenderer::drawField(Field* f)
{
    /*
    intRect_t& curRect = f.getDrawRect();
    disableTexture();
    imm.color3f(UI_FG_R, UI_FG_G, UI_FG_B);
    imm.vertex2i(curRect.x, curRect.y);
    imm.vertex2i(curRect.x + curRect.w, curRect.y);
    imm.vertex2i(curRect.x + curRect.w, curRect.y + curRect.h);
    imm.vertex2i(curRect.x, curRect.y + curRect.h);
    imm.color3f(UI_BG_R, UI_BG_G, UI_BG_B);
    imm.vertex2i(curRect.x + 1, curRect.y + 1);
    imm.vertex2i(curRect.x + curRect.w - 1, curRect.y + 1);
    imm.vertex2i(curRect.x + 1, curRect.y + curRect.h - 1);
    drawString(f.getText(), curRect.x + f.getTextLoc().x, curRect.y + f.getTextLoc().y, f.getFontScale(), UI_FG_R, UI_FG_G, UI_FG_B);
     */
}

void UIRenderer::drawButton(Button* b)
{
    /*
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
    imm.color3f(UI_BG_R, UI_BG_G, UI_BG_B);
    imm.vertex2i(rect.x + BORDER_WIDTH, rect.y + BORDER_WIDTH);
    imm.vertex2i(rect.x + rect.w - BORDER_WIDTH, rect.y + BORDER_WIDTH);
    imm.vertex2i(rect.x + rect.w - BORDER_WIDTH, rect.y + rect.h - BORDER_WIDTH);
    imm.vertex2i(rect.x + BORDER_WIDTH, rect.y + rect.h - BORDER_WIDTH);
    imm.color3f(UI_FG_R * colorMult, UI_FG_G * colorMult, UI_FG_B * colorMult);
    imm.vertex2i(rect.x, rect.y);
    imm.vertex2i(rect.x + rect.w, rect.y);
    imm.vertex2i(rect.x + rect.w - BORDER_WIDTH, rect.y + BORDER_WIDTH);
    imm.vertex2i(rect.x + BORDER_WIDTH, rect.y + BORDER_WIDTH);
    imm.vertex2i(rect.x, rect.y);
    imm.vertex2i(rect.x + BORDER_WIDTH, rect.y + BORDER_WIDTH);
    imm.vertex2i(rect.x + BORDER_WIDTH, rect.y + rect.h - BORDER_WIDTH);
    imm.vertex2i(rect.x, rect.y + rect.h);
    imm.color3f(UI_FG_R * colorMult * SHADE, UI_FG_G * colorMult * SHADE, UI_FG_B * colorMult * SHADE);
    imm.vertex2i(rect.x + rect.w, rect.y);
    imm.vertex2i(rect.x + rect.w, rect.y + rect.h);
    imm.vertex2i(rect.x + rect.w - BORDER_WIDTH, rect.y + rect.h - BORDER_WIDTH);
    imm.vertex2i(rect.x + rect.w - BORDER_WIDTH, rect.y + BORDER_WIDTH);
    imm.vertex2i(rect.x + BORDER_WIDTH, rect.y + rect.h - BORDER_WIDTH);
    imm.vertex2i(rect.x + rect.w - BORDER_WIDTH, rect.y + rect.h - BORDER_WIDTH);
    imm.vertex2i(rect.x + rect.w, rect.y + rect.h);
    imm.vertex2i(rect.x, rect.y + rect.h);
    drawString(b.getText(), b.getTextLoc().x + rect.x, b.getTextLoc().y + rect.y, b.getFontScale(), UI_FG_R, UI_FG_G, UI_FG_B);
     */
}

void UIRenderer::drawScrollBlock(ScrollBlock* sb)
{
    /*
    imm.color3f(UI_BG_R * SHADE, UI_BG_G * SHADE, UI_BG_B * SHADE);
    disableTexture();
    intRect_t& sbrect = sb.getDrawRect();
    imm.vertex2i(sbrect.x, sbrect.y);
    imm.vertex2i(sbrect.x + sbrect.w, sbrect.y);
    imm.vertex2i(sbrect.x + sbrect.w, sbrect.y + sbrect.h);
    imm.vertex2i(sbrect.x, sbrect.y + sbrect.h);
    //For some reason glScissor wants (x, y) to be lower-left corner
    if(sb.hasBar())
    {
        intRect_t bar = sb.getBarRect();
        imm.color3f(UI_FG_R, UI_FG_G, UI_FG_B);
        imm.vertex2i(bar.x, bar.y);
        imm.vertex2i(bar.x + bar.w, bar.y);
        imm.vertex2i(bar.x + bar.w, bar.y + bar.h);
        imm.vertex2i(bar.x, bar.y + bar.h);
    }
     */
}

void UIRenderer::drawDraggable(Draggable* d)
{
    //TODO
}

void UIRenderer::drawMinimap(Minimap *mm)
{
    
}
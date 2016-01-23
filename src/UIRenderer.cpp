#include "UIRenderer.h"

using namespace std;

UIRenderer::UIRenderer() :
QUAD_INIT(1000),
LINE_INIT(100),
imm(QUAD_INIT, LINE_INIT),
fg(190, 190, 190, 255),
bg(120, 120, 120, 255),
BORDER_WIDTH(3),
PAD(5),
BAR_WIDTH(6),
SHADE(0.85)
{
    GLERR
    imm = ImmediateDraw(QUAD_INIT, LINE_INIT);
    PRINT("Created UI renderer.");
}

void UIRenderer::draw()
{
    glDisable(GL_DEPTH_TEST);
    imm.beginFrame();
    drawScene(GUI::current);
    drawScene(GUI::debugScene);
    imm.blit("cursor", Input::mouseX, Input::mouseY);
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
    //only draw if there is more than 1 option
    if(ms->getNumOptions() > 0)
    {
        vector<string>& list = ms->getOptions();
        auto rect = ms->getScreenRect();
        //fill whole rectangle with background color
        imm.drawRect(bg, rect);
        //if an option is selected, draw foreground rectangle around that
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
            Rectangle optRect(rect.x + PAD, rect.y + PAD + i * ms->getOptHeight(), rect.w - PAD * 2, ms->getOptHeight() - PAD * 2);
            imm.color3b(fg.r, fg.g, fg.b);
            imm.drawStringAuto(list[i], optRect, Justify::LEFT_JUST);
        }
    }
}

void UIRenderer::drawLabel(Label* l)
{
    auto lrect = l->getScreenRect();
    imm.color3b(fg.r, fg.g, fg.b);
    imm.drawStringAuto(l->getText(), lrect, Justify::CENTER_JUST);
}

void UIRenderer::drawField(Field* f)
{
    auto rect = f->getScreenRect();
    //Fill with background
    imm.drawRect(bg, rect);
    //draw lines around border
    imm.drawLineRect(fg, rect);
    //draw text
    Rectangle textRect(rect.x + PAD, rect.y + PAD, rect.w - 2 * PAD, rect.h - 2 * PAD);
    imm.drawStringAuto(f->getText(), textRect, Justify::LEFT_JUST);
}

void UIRenderer::drawButton(Button* b)
{
    auto rect = b->getScreenRect();
    //Draw the entire button lighter if mouse is over
    bool lit = b->isMouseOver();
    Color4 light = fg;
    Color4 dark = fg;
    Color4 background = bg;  //color of background field
    if(lit)
    {
        light = light * (1 / SHADE);
        background = background * (1 / SHADE);
    }
    else
    {
        dark = dark * SHADE;
    }
    //Draw background
    imm.drawRect(background, rect);
    //Draw beveled frame
    imm.drawBevelFrame(light, dark, rect, BORDER_WIDTH);
    //Draw text
    Rectangle textRect(rect.x + PAD, rect.y + PAD, rect.w - PAD * 2, rect.h - PAD * 2);
    imm.drawStringAuto(b->getText(), textRect, light);
}

void UIRenderer::drawScrollBlock(ScrollBlock* sb)
{
    auto rect = sb->getScreenRect();
    imm.drawRect(bg * SHADE, rect);
    Rectangle barRect;
    auto barH = sb->getBarHeight();
    auto barY = sb->getBarPos();
    barRect.x = rect.x + rect.w - PAD - BAR_WIDTH;
    barRect.y = rect.y + PAD + barY * (rect.h - PAD * 2);
    barRect.w = BAR_WIDTH;
    barRect.h = (rect.h - PAD * 2) * barH;
    imm.drawRect(fg, rect);
    //imm.scissorRect(rect);
    //imm.enableScissorTest();
    for(auto c : sb->getChildren())
        drawComponent(c);
    //imm.disableScissorTest();
}

void UIRenderer::drawDraggable(Draggable* d)
{
    //TODO
}

void UIRenderer::drawMinimap(Minimap *mm)
{
    imm.drawBevelFrame(fg, fg * SHADE, mm->getScreenRect(), BORDER_WIDTH);
    auto interior = mm->getScreenRect();
    interior.x += BORDER_WIDTH;
    interior.y += BORDER_WIDTH;
    interior.w -= 2 * BORDER_WIDTH;
    interior.h -= 2 * BORDER_WIDTH;
    imm.blit("minimap", interior);
    if(Input::keystate[SDL_SCANCODE_T])
        imm.blit("topo", interior);
}
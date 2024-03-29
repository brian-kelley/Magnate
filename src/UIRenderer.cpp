#include "UIRenderer.h"

using namespace std;
using namespace ImmediateDraw;

//Aesthetic constants
const Color4 UIRenderer::fg(190, 190, 190, 255);
const Color4 UIRenderer::bg(120, 120, 120, 255);
const int UIRenderer::BORDER_WIDTH = 3;
const int UIRenderer::PAD = 5;
const int UIRenderer::BAR_WIDTH = 6;
const float UIRenderer::SHADE = 0.85;

void UIRenderer::draw()
{
    glDisable(GL_DEPTH_TEST);
    drawScene(GUI::current);
    drawScene(GUI::debugScene);
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
        drawRect(bg, rect);
        //if an option is selected, draw foreground rectangle around that
        if(ms->getSelection() != -1)
        {
            color3b(fg.r, fg.g, fg.b);
            disableTextures();
            int optY = rect.y + ms->getSelection() * ms->getOptHeight();
            vertex2i(rect.x, optY);
            vertex2i(rect.x + rect.w, optY);
            vertex2i(rect.x + rect.w, optY + PAD);
            vertex2i(rect.x, optY + PAD);
            vertex2i(rect.x, optY + PAD);
            vertex2i(rect.x + PAD, optY + PAD);
            vertex2i(rect.x + PAD, optY + ms->getOptHeight() - PAD);
            vertex2i(rect.x, optY + ms->getOptHeight() - PAD);
            vertex2i(rect.x + rect.w - PAD, optY + PAD);
            vertex2i(rect.x + rect.w, optY + PAD);
            vertex2i(rect.x + rect.w, optY + ms->getOptHeight() - PAD);
            vertex2i(rect.x + rect.w - PAD, optY + ms->getOptHeight() - PAD);
            vertex2i(rect.x, optY + ms->getOptHeight() - PAD);
            vertex2i(rect.x + rect.w, optY + ms->getOptHeight() - PAD);
            vertex2i(rect.x + rect.w, optY + ms->getOptHeight());
            vertex2i(rect.x, optY + ms->getOptHeight());
        }
        for(int i = 0; i < int(list.size()); i++)
        {
            Rectangle optRect(rect.x + PAD, rect.y + PAD + i * ms->getOptHeight(), rect.w - PAD * 2, ms->getOptHeight() - PAD * 2);
            color3b(fg.r, fg.g, fg.b);
            drawStringAuto(list[i], optRect, Justify::LEFT_JUST);
        }
    }
}

void UIRenderer::drawLabel(Label* l)
{
    auto lrect = l->getScreenRect();
    color3b(fg.r, fg.g, fg.b);
    drawStringAuto(l->getText(), lrect, Justify::CENTER_JUST);
}

void UIRenderer::drawField(Field* f)
{
    auto rect = f->getScreenRect();
    //Fill with background
    drawRect(bg, rect);
    //draw lines around border
    drawLineRect(fg, rect);
    //draw text
    Rectangle textRect(rect.x + PAD, rect.y + PAD, rect.w - 2 * PAD, rect.h - 2 * PAD);
    drawStringAuto(f->getText(), textRect, Justify::LEFT_JUST);
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
        dark = dark * SHADE;
    //Draw background
    drawRect(background, rect);
    //Draw beveled frame
    drawBevelFrame(light, dark, rect, BORDER_WIDTH);
    //Draw text
    Rectangle textRect(rect.x + PAD, rect.y + PAD, rect.w - PAD * 2, rect.h - PAD * 2);
    drawStringAuto(b->getText(), textRect, light);
}

void UIRenderer::drawScrollBlock(ScrollBlock* sb)
{
    auto rect = sb->getScreenRect();
    drawRect(bg * SHADE, rect);
    Rectangle barRect;
    auto barH = sb->getBarHeight();
    auto barY = sb->getBarPos();
    barRect.x = rect.x + rect.w - PAD - BAR_WIDTH;
    barRect.y = rect.y + PAD + barY * (rect.h - PAD * 2);
    barRect.w = BAR_WIDTH;
    barRect.h = (rect.h - PAD * 2) * barH;
    drawRect(fg, barRect);
    scissorRect(rect);
    enableScissorTest();
    for(auto c : sb->getChildren())
        drawComponent(c);
    disableScissorTest();
}

void UIRenderer::drawDraggable(Draggable* d)
{
    //TODO
}

void UIRenderer::drawMinimap(Minimap *mm)
{
    auto& rect = mm->getScreenRect();
    color3b(255, 255, 255);
    blit("minimap", rect);
    if(Input::keystate[SDL_SCANCODE_T])
        blit("topo", rect);
    drawLineRect(fg, rect);
    auto fc = Camera::getFrustumCorners(Camera::viewMat, Camera::projMat);
    disableTextures();
    setColor(fg);
    float mult = float(mm->getScreenRect().w) / GlobalConfig::WORLD_SIZE;
    Pos2 origin(rect.x, rect.y);
    auto p1 = Coord::worldToTile(fc.upperLeft) * mult + origin;
    auto p2 = Coord::worldToTile(fc.upperRight) * mult + origin;
    auto p3 = Coord::worldToTile(fc.lowerRight) * mult + origin;
    auto p4 = Coord::worldToTile(fc.lowerLeft) * mult + origin;
    lineVertex2i(p1);
    lineVertex2i(p2);
    lineVertex2i(p2);
    lineVertex2i(p3);
    lineVertex2i(p3);
    lineVertex2i(p4);
    lineVertex2i(p1);
    lineVertex2i(p4);
}

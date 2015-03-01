//
//  View.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/20/14293.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "View.h"

using namespace std;
using namespace constants;
using namespace view;
using namespace boost::filesystem;

int view::scrX = 0;
int view::scrY = 0;
SDL_Window* view::window;
SDL_Renderer* view::renderer;
SDL_GLContext view::context;
Atlas* view::mainAtlas;

void view::init(int screenX, int screenY)
{
    scrX = screenX;
    scrY = screenY;
    initSDLVideo();
    configGL();
    initAtlas();
}

void view::dispose()
{
    delete mainAtlas;
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    SDL_DestroyWindow(window);
    window = nullptr;
    IMG_Quit();
    SDL_Quit();
}

void view::prepareFrame()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void view::finalizeFrame()
{
    SDL_GL_SwapWindow(window);
}

void view::drawBuilding(Building& b)
{
    //b's cuboids are already in correct order for rendering (back to front)
    for(int i = 0; i < b.numCuboids(); i++)
    {
        drawCuboid(b.getCuboidRef(i));
    }
}

void view::drawWorld(World& currentWorld)   //probably too general of a function
{

}

void view::drawComponent(Component& c)
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

void view::drawMultiSelect(MultiSelect& ms)
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

void view::drawLabel(Label& l)
{
    intRect_t& lrect = l.getDrawRect();
    drawString(l.getText(), lrect.x + l.getTextLoc().x, lrect.y + l.getTextLoc().y, l.getFontScale(), UI_FG_R, UI_FG_G, UI_FG_B);
}

void view::drawField(Field& f)
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

void view::drawButton(Button& b)
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

void view::drawScrollBlock(ScrollBlock& sb)
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

void view::configGL()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(glGetError() != GL_NO_ERROR)
    {
        cout << "Couldn't initialize GL_PROJECTION mode!" << endl;
        exit(1);
    }
    glOrtho(0, constants::WINDOW_W, constants::WINDOW_H, 0, 1, -1);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_TEXTURE_2D);
    glClearColor(1, 1, 1, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void view::updateWindowSize()
{
    SDL_GetWindowSize(window, &constants::WINDOW_W, &constants::WINDOW_H);
}

void view::blit(int index, int x, int y)
{
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(mainAtlas->tileX(index), mainAtlas->tileY(index));
    glVertex2i(x, y);
    glTexCoord2f(mainAtlas->tileX(index) + mainAtlas->tileW(index),
                 mainAtlas->tileY(index));
    glVertex2i(x + mainAtlas->tileW(index), y);
    glTexCoord2f(mainAtlas->tileX(index) + mainAtlas->tileW(index),
                 mainAtlas->tileY(index) + mainAtlas->tileH(index));
    glVertex2i(x + mainAtlas->tileW(index),
               y + mainAtlas->tileH(index));
    glTexCoord2f(mainAtlas->tileX(index),
                 mainAtlas->tileY(index) + mainAtlas->tileH(index));
    glVertex2i(x, y + mainAtlas->tileH(index));
    glEnd();
}

void view::blit(int index, int x1, int y1, int x2, int y2)
{
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(mainAtlas->tileX(index), mainAtlas->tileY(index));
    glVertex2i(x1, y1);
    glTexCoord2f(mainAtlas->tileX(index) + mainAtlas->tileW(index),
                 mainAtlas->tileY(index));
    glVertex2i(x2, y1);
    glTexCoord2f(mainAtlas->tileX(index) + mainAtlas->tileW(index),
                 mainAtlas->tileY(index) + mainAtlas->tileH(index));
    glVertex2i(x2, y2);
    glTexCoord2f(mainAtlas->tileX(index),
                 mainAtlas->tileY(index) + mainAtlas->tileH(index));
    glVertex2i(x1, y2);
    glEnd();
}

void view::drawString(string text, int x, int y)
{
    drawString(text, x, y, 1, 0, 0, 0);
}

void view::drawString(string text, int x, int y, float scale)
{
    drawString(text, x, y, scale, 0, 0, 0);
}

void view::drawString(string text, int x, int y, float scale, float r, float g, float b)
{
    glColor3f(r, g, b);
    glEnable(GL_BLEND);
    for(int i = 0; i < text.size(); i++)
    {
        if(text[i] != ' ')
        {
            blit(mainAtlas->tileFromChar(text[i]),
                       x + i * constants::FONTW * scale, y,
                       x + (1 + i) * constants::FONTW * scale, y + constants::FONTH * scale);
        }
    }
}

void view::drawCuboid(Cuboid& c)
{
    int posX = ix(c.getX(), c.getY()) - scrX;
    int posY = jy(c.getX(), c.getY()) + (int) (constants::HMULT * c.getZ()) - scrY;
    //Left wall
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(mainAtlas->tileX(c.getLeft()),
                 mainAtlas->tileY(c.getLeft()));
    glVertex2i(posX + c.draw.x2, posY + c.draw.y2);
    glTexCoord2f(mainAtlas->tileX(c.getLeft()) + mainAtlas->tileW(c.getLeft()),
                 mainAtlas->tileY(c.getLeft()));
    glVertex2i(posX + c.draw.x3, posY + c.draw.y3);
    glTexCoord2f(mainAtlas->tileX(c.getLeft()) + mainAtlas->tileW(c.getLeft()),
                 mainAtlas->tileY(c.getLeft()) + mainAtlas->tileH(c.getLeft()));
    glVertex2i(posX + c.draw.x4, posY + c.draw.y4);
    glTexCoord2f(mainAtlas->tileX(c.getLeft()) + mainAtlas->tileW(c.getLeft()),
                 mainAtlas->tileY(c.getLeft()) + mainAtlas->tileH(c.getLeft()));
    glVertex2i(posX, posY);
    glEnd();
    //Roof
    glBegin(GL_QUADS);
    glTexCoord2f(mainAtlas->tileX(c.getRoof()),
                 mainAtlas->tileY(c.getRoof()));
    glVertex2i(posX + c.draw.x2, posY + c.draw.y2);
    glTexCoord2f(mainAtlas->tileX(c.getRoof()) + mainAtlas->tileW(c.getRoof()),
                 mainAtlas->tileY(c.getRoof()));
    glVertex2i(posX + c.draw.x5, posY + c.draw.y5);
    glTexCoord2f(mainAtlas->tileX(c.getRoof()) + mainAtlas->tileW(c.getRoof()),
                 mainAtlas->tileY(c.getRoof()) + mainAtlas->tileH(c.getRoof()));
    glVertex2i(posX + c.draw.x6, posY + c.draw.y6);
    glTexCoord2f(mainAtlas->tileX(c.getRoof()),
                 mainAtlas->tileY(c.getRoof()) + mainAtlas->tileH(c.getRoof()));
    glVertex2i(posX + c.draw.x3, posY + c.draw.y3);
    glEnd();
    //Right wall
    glColor3f(constants::SHADE, constants::SHADE, constants::SHADE);
    glBegin(GL_QUADS);
    glTexCoord2f(mainAtlas->tileX(c.getRight()),
                 mainAtlas->tileY(c.getRight()));
    glVertex2i(posX + c.draw.x3, posY + c.draw.y3);
    glTexCoord2f(mainAtlas->tileX(c.getRight()) + mainAtlas->tileW(c.getRight()),
                 mainAtlas->tileY(c.getRight()));
    glVertex2i(posX + c.draw.x6, posY + c.draw.y6);
    glTexCoord2f(mainAtlas->tileX(c.getRight()) + mainAtlas->tileW(c.getRight()),
                 mainAtlas->tileY(c.getRight()) + mainAtlas->tileH(c.getRight()));
    glVertex2i(posX + c.draw.x7, posY + c.draw.y7);
    glTexCoord2f(mainAtlas->tileX(c.getRight()),
                 mainAtlas->tileY(c.getRight()) + mainAtlas->tileH(c.getRight()));
    glVertex2i(posX + c.draw.x4, posY + c.draw.y4);
    glEnd();
}

void view::initSDLVideo()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        cout << "Failed to SDL video." << endl;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    window = SDL_CreateWindow("Magnate", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              constants::WINDOW_W, constants::WINDOW_H, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if(!window)
    {
        cout << "Failed to create SDL window." << endl;
        exit(1);
    }
    context = SDL_GL_CreateContext(window);
    if(context == nullptr)
    {
        cout << "Error creating GL context." << endl;
        exit(1);
    }
    SDL_GL_SetSwapInterval(1);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void view::initAtlas()
{
    path imgPath = initial_path() / BIN_TO_ROOT / "assets" / "main_atlas.png";
    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags) & imgFlags))
    {
        cout << "Error: View failed to initialize image loading." << endl;
        cout << "Not going to attempt loading main atlas." << endl;
        exit(1);
    }
    else
    {
        mainAtlas = new Atlas("main", renderer);
        if(mainAtlas == nullptr)
        {
            cout << "Error: Failed to create atlas." << endl;
        }
    }
    mainAtlas->bind();
}

//
//  View.cpp
//  MagIndev
//
//  Created by Brian Kelley on 10/20/14293.
//  Copyright (c) 2014 Brian Kelley. All rights reserved.
//

#include "View.h"

using namespace std;

View::View(int screenX, int screenY)
: scrX(screenX), scrY(screenY)
{
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        cout << "Failed to SDL video." << endl;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    window = SDL_CreateWindow("Magnate", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              constants::WINDOW_W, constants::WINDOW_H, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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
    this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);
    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags) & imgFlags))
    {
        cout << "Error: View failed to initialize image loading." << endl;
        cout << "Not going to attempt loading main atlas." << endl;
        exit(1);
    }
    else
    {
        mainAtlas = new Atlas("main", this->renderer);
        if(mainAtlas == nullptr)
        {
            cout << "Error: Failed to create atlas." << endl;
        }
    }
    configGL();
    mainAtlas->bind();
    model = new Model();
}

View::~View()
{
    delete model;
    model = nullptr;
    delete mainAtlas;
    SDL_DestroyRenderer(this->renderer);
    this->renderer = nullptr;
    SDL_DestroyWindow(this->window);
    this->window = nullptr;
    IMG_Quit();
    SDL_Quit();
}
//Called in main loop
void View::update()
{
    glClear(GL_COLOR_BUFFER_BIT);
    model->update();
    glEnable(GL_TEXTURE_2D);
    this->drawString("This is a string.", 100, 100, 0.5, 1, 0.5, 0);
    this->drawString("This is another string.", 100, 150, 0.5, 1, 0.5, 0);
    SDL_GL_SwapWindow(this->window);
}

void View::drawCuboid(Cuboid& c)
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

void View::drawBuilding(Building& b)
{
    //b's cuboids are already in correct order for rendering
    for(int i = 0; i < b.numCuboids(); i++)
    {
        this->drawCuboid(b.getCuboidRef(i));
    }
}

void View::drawScene(Scene& s)
{
    for(int i = 0; i < s.getButtons()->size(); i++)
    {
        drawButton(s.getButtons()->at(i));
    }
    for(int i = 0; i < s.getLabels()->size(); i++)
    {
        drawLabel(s.getLabels()->at(i));
    }
    for(int i = 0; i < s.getFields()->size(); i++)
    {
        drawField(s.getFields()->at(i));
    }
}

void View::blit(int index, int x, int y)
{
    cout << mainAtlas->tileW(index) << " " << mainAtlas->tileH(index) << endl;
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

void View::blit(int index, int x1, int y1, int x2, int y2)
{
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

void View::drawString(string text, int x, int y)
{
    this->drawString(text, x, y, 1.0f, 1.0f, 1.0f, 1.0f);
}

void View::drawString(string text, int x, int y, float scale)
{
    this->drawString(text, x, y, scale, 1.0f, 1.0f, 1.0f);
}

void View::drawString(string text, int x, int y, float scale, float r, float g, float b)
{
    glColor3f(r, g, b);
    for(int i = 0; i < text.size(); i++)
    {
        if(text[i] != ' ')
        {
            this->blit(mainAtlas->tileFromChar(text[i]),
                       x + i * constants::FONTW * scale, y,
                       x + (1 + i) * constants::FONTW * scale, y + constants::FONTH * scale);
        }
    }
}

void View::drawLabel(Label& l)
{
    
}

void View::drawField(Field &f)
{
    
}

void View::drawButton(Button &b)
{
    SDL_Rect* curRect = &b.getRect();
    glDisable(GL_TEXTURE_2D);
    glColor4f(constants::UI_BG_R, constants::UI_BG_G, constants::UI_BG_B, 1);
    glBegin(GL_QUADS);
    glVertex2i(curRect->x + constants::BORDER_WIDTH, curRect->y + constants::BORDER_WIDTH);
    glVertex2i(curRect->x + curRect->w - constants::BORDER_WIDTH, curRect->y + constants::BORDER_WIDTH);
    glVertex2i(curRect->x + curRect->w - constants::BORDER_WIDTH, curRect->y + curRect->h - constants::BORDER_WIDTH);
    glVertex2i(curRect->x + constants::BORDER_WIDTH, curRect->y + curRect->h - constants::BORDER_WIDTH);
    glEnd();
    glColor4f(constants::UI_FG_R, constants::UI_FG_G, constants::UI_FG_B, 1);
    glBegin(GL_POLYGON);
    glVertex2i(curRect->x, curRect->y);
    glVertex2i(curRect->x + curRect->w, curRect->y);
    glVertex2i(curRect->x + curRect->w - constants::BORDER_WIDTH,
               curRect->y + constants::BORDER_WIDTH);
    glVertex2i(curRect->x + constants::BORDER_WIDTH,
               curRect->y + constants::BORDER_WIDTH);
    glVertex2i(curRect->x + constants::BORDER_WIDTH,
               curRect->y + curRect->h - constants::BORDER_WIDTH);
    glVertex2i(curRect->x, curRect->y + curRect->h);
    glEnd();
    glColor4f(constants::UI_FG_R * constants::SHADE,
              constants::UI_FG_G * constants::SHADE,
              constants::UI_FG_B * constants::SHADE, 1);
    glBegin(GL_QUADS);
    glVertex2i(curRect->x, curRect->y + curRect->h);
    glVertex2i(curRect->x + constants::BORDER_WIDTH, curRect->y + curRect->h - constants::BORDER_WIDTH);
    glVertex2i(curRect->x + curRect->w - constants::BORDER_WIDTH,
               curRect->y + curRect->h - constants::BORDER_WIDTH);
    glVertex2i(curRect->x + curRect->w, curRect->y + curRect->h);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2i(curRect->x + curRect->w, curRect->y);
    glVertex2i(curRect->x + curRect->w, curRect->y + curRect->h);
    glVertex2i(curRect->x + curRect->w - constants::BORDER_WIDTH,
               curRect->y + curRect->h - constants::BORDER_WIDTH);
    glVertex2i(curRect->x + curRect->w - constants::BORDER_WIDTH,
               curRect->y + constants::BORDER_WIDTH);
    glEnd();
    glEnable(GL_TEXTURE_2D);
    this->drawString(b.getText(), b.getTextLoc().x + curRect->x, b.getTextLoc().y + curRect->y, b.getFontScale(),
                     constants::UI_FG_R, constants::UI_FG_G, constants::UI_FG_B);
}

void View::configGL()
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
    glBlendEquation(GL_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void View::updateWindowSize()
{
    SDL_GetWindowSize(this->window, &constants::WINDOW_W, &constants::WINDOW_H);
}
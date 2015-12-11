#ifndef __TEXGEN_H__
#define __TEXGEN_H__

#include <iostream>
#include "Atlas.h"
#include "Heightmap.h"
#include "RandomUtils.h"

namespace TexGen
{
    struct Color
    {
        Color();
        Color(byte r, byte g, byte b, byte a);
        byte b;
        byte g;
        byte r;
        byte a;
        static const int BPP = 4;
    };
    
    bool operator==(const Color& c1, const Color& c2);
    
    struct Buffer
    {
    public:
        Buffer(int length, int width);
        Buffer(Buffer& b);
        ~Buffer();
        //Set and get pixels (w/bounds checking)
        Color get(int x, int y);
        void set(int x, int y, Color c);
        //Get the average color in 3x3.
        Color getAvg(int x, int y);
        //Avg of most common color(s) in 3x3.
        Color getMode(int x, int y);
        void smooth(int iters);
        void fill(Color c);
        void fillNoAlpha(Color c);
        void makeOpaque();
        int len;
        int wid;
        Color* data;
        //Internal utilities
    private:
        bool validPx(int x, int y);
        Color calcColor(Color avg, int size, double rough);
    };
    class Generator
    {
    public:
        void copyToAtlas(int texID);
    protected:
        Generator(int x, int y);
        virtual void generate() = 0;
        Buffer buf;
    };
    
    class Cloud : public Generator
    {
    public:
        Cloud(int x, int y);
        void generate();
    };
    
    class Plains : public Generator
    {
    public:
        Plains(int x, int y);
        void generate();
    };
    
    class Forest : public Generator
    {
    public:
        Forest(int x, int y);
        void generate();
    };
}

#endif
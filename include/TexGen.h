#ifndef __TEXGEN_H__
#define __TEXGEN_H__

#include <iostream>
#include "Atlas.h"
#include "Heightmap.h"
#include "RandomUtils.h"
#include "GenTypes.h"

namespace TexGen
{
    bool operator==(const Color4& c1, const Color4& c2);
    
    struct Buffer
    {
    public:
        Buffer(int length, int width);
        Buffer(Buffer& b);
        ~Buffer();
        //Set and get pixels (w/bounds checking)
        Color4 get(int x, int y);
        void set(int x, int y, Color4 c);
        //Get the average color in 3x3.
        Color4 getAvg(int x, int y);
        //Avg of most common color(s) in 3x3.
        Color4 getMode(int x, int y);
        void smooth(int iters);
        void fill(Color4 c);
        void fillNoAlpha(Color4 c);
        void makeOpaque();
        int len;
        int wid;
        Color4* data;
        //Internal utilities
    private:
        bool validPx(int x, int y);
        Color4 calcColor(Color4 avg, int size, double rough);
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
#include "TexManager.h"

using namespace std;

void TexManager::init()
{
    cout << "Generating textures." << endl;
    {
        TexGen::Forest f(512, 512);
        f.generate();
        f.copyToAtlas(Atlas::tileFromName("forestL0"));
    }
    {
        TexGen::Cloud c(512, 512);
        c.generate();
        c.copyToAtlas(Atlas::tileFromName("clouds"));
    }
    {
        TexGen::Plains p(512, 512);
        p.generate();
        p.copyToAtlas(Atlas::tileFromName("plainsL0"));
    }
}
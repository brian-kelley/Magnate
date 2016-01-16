#ifndef __SCENE_H__
#define __SCENE_H__

#include "Component.h"
#include "GlobalConfig.h"

class Scene : public Component
{
public:
    Scene();
    CompType getType();
    void setWidth(int w);
    void setHeight(int h);
    void processResize();
};

#endif
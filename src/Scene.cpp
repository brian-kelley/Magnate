#include "Scene.h"

using namespace std;

Scene::Scene() : Component(0, 0, GlobalConfig::WINDOW_W, GlobalConfig::WINDOW_H, StickyDirs::top | StickyDirs::bottom | StickyDirs::left | StickyDirs::right, false, nullptr) {}

CompType Scene::getType()
{
    return CompType::scene;
}

void Scene::setWidth(int w)
{
    local.w = w;
    updateScreenRect();
}

void Scene::setHeight(int h)
{
    local.h = h;
    updateScreenRect();
}

void Scene::processResize()
{
    //new width/height of window must manually be set
    for(auto c : children)
    {
        c->processResize();
    }
}

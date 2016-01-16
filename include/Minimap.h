#ifndef __MINIMAP_H__
#define __MINIMAP_H__

#include <iostream>
#include "Component.h"
#include "Scene.h"
#include "Terrain.h"
#include "Coord.h"
#include "GlmHeaders.h"
#include "Atlas.h"
#include "GenTypes.h"
#include "GlobalConfig.h"
#include "Heightmap.h"
#include "Topo.h"
#include "Camera.h"
#include "Input.h"
#include "World.h"

class Minimap : public Component
{
public:
    Minimap(Scene* gameScene);
    static void buildTexture();
private:
    void update();  //set screenX/screenY based on mouseX/mouseY
    static void plotPosOnMap(glm::vec2 worldPos); //also call vertex2i
    static void putMinimapPixel(int x, int y, Uint32* buf, int maxHeight);
    static Color4 colorFromTerrain(Ground g);
    static int MINIMAP_SIZE; //pixels in texture
    static int MINIMAP_BORDER;//amount of space between edge of window and edge o
    static glm::mat4 tileToMinimap;
    static glm::mat4 minimapToTile;
    static glm::vec4 mapToWorld(Pos2 mapPos);
};

#endif
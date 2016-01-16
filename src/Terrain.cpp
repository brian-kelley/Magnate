#include "Terrain.h"

using namespace std;

string Terrain::getTextureName(Ground g)
{
    switch(g)
    {
        case Ground::WATER:
        case Ground::RIVER:
        case Ground::LAKE:
            return "water";
        case Ground::FLOODPLAINS:
            return "floodplains";
        case Ground::CONIFER_FOREST:
        case Ground::DECID_FOREST:
        case Ground::OUTLET_SEARCHED:
            return "forest";
        case Ground::PLAINS:
            return "grass";
        case Ground::DESERT:
        case Ground::BEACH:
            return "sand";
        case Ground::MOUNTAINS:
        case Ground::STONE:
            return "mountains";
        case Ground::SNOWCAP:
            return "snow";
        case Ground::TAIGA:
            return "taiga";
        case Ground::RAINFOREST:
            return "rainforest";
        case Ground::TUNDRA:
            return "tundra";
        default:
            return "test1";
    }
}
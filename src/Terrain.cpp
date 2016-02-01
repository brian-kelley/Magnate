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
        case Ground::DECID_FOREST:
            return "forest";
        case Ground::PLAINS:
            return "grass";
        case Ground::SAVANNA:
            return "savanna";
        case Ground::DESERT:
        case Ground::BEACH:
            return "sand";
        case Ground::MOUNTAINS:
            return "mountains";
        case Ground::SNOWCAP:
            return "snow";
        case Ground::BOREAL_FOREST:
            return "taiga";
        case Ground::RAINFOREST:
            return "rainforest";
        case Ground::TUNDRA:
            return "tundra";
        default:
            return "test1";
    }
}

string Terrain::getName(Ground g)
{
    switch(g)
    {
        case Ground::WATER:
            return "Ocean";
        case Ground::MOUNTAINS:
            return "Mountains";
        case RIVER:
            return "River";
        case Ground::LAKE:
            return "Lake";
        case Ground::RAINFOREST:
            return "Rainforest";
        case Ground::BOREAL_FOREST:
            return "Boreal forest";
        case Ground::DECID_FOREST:
            return "Deciduous forest";
        case Ground::TUNDRA:
            return "Tundra";
        case Ground::DESERT:
            return "Desert";
        case Ground::SNOWCAP:
            return "Snow caps";
        case Ground::BEACH:
            return "Beach";
        case Ground::PLAINS:
            return "Plains";
        case Ground::SAVANNA:
            return "Savanna";
        default:
            return "???";
    }
}
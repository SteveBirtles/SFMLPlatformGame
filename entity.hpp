#pragma once

#include "tilemap.hpp"

#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

class Entity
{

private:
    Sprite* sprite;
    double dx;
    double dy;
    double direction_dx;
    TileType tile;

public:
    Entity(Texture& t);

    double getX();
    double getY();
    double getDX();
    double getDY();

    Sprite& getSprite();

    void setPosition(double _x, double _y);
    void setVelocity(double _dx, double _dy);

    void updatePosition(double frameLength, TileMap& tileMap);
    void updateDirection(double d);
    static void processEntityCollisions(vector<Entity*> entities, double frameLength);

    TileType onTile();
};

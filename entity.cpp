#include <SFML/Graphics.hpp>

#include "entity.hpp"
#include "tilemap.hpp"

#include <iostream>
#include <sstream>

#define str(x) dynamic_cast< std::ostringstream & >( \
       ( std::ostringstream() << std::dec << x ) ).str()

using namespace std;
using namespace sf;

Entity::Entity(Texture& t)
{
    sprite = new Sprite();
    sprite -> setTexture(t);
    sprite -> setPosition(0, 0);
    sprite -> setOrigin(t.getSize().x / 2, t.getSize().y / 2);
    dx = 0;
    dy = 0;
    direction_dx = 1;
}

double Entity::getX()
{
    return sprite -> getPosition().x;
}

double Entity::getY()
{
    return sprite -> getPosition().y;
}

double Entity::getDX()
{
    return dx;
}

double Entity::getDY()
{
    return dy;
}

Sprite& Entity::getSprite()
{
    return *sprite;
}

void Entity::setPosition(double _x, double _y)
{
    sprite -> setPosition(_x, _y);
}

void Entity::setVelocity(double _dx, double _dy)
{
    dx = _dx;
    dy = _dy;
}


void Entity::updateDirection(double d)
{
    if (d != 0) direction_dx = d;
}


void Entity::updatePosition(double frameLength, TileMap& tileMap)
{

    double cor = 0.25;
    double cof = 0.05;
    enum class direction { none, left, right, top, bottom };
    double iota = -0.0001;

    double s = tileMap.tileSize();

    bool vertCol = false;
    bool horizCol = false;
    double deltaT = frameLength;

    double x = getX();
    double y = getY();

    tile = TileType::BACKGROUND;

    double gravity = 2000;
    double maxSpeed = 1500;

    if (tileMap.getTileType(x/s, y/s) == TileType::FOREGROUND_LIQUID)
    {
        tile = TileType::FOREGROUND_LIQUID;
        dx *= 1-cof;
        dy *= 1-2*cof;
        dy += (gravity/4) * frameLength;
    }
    else if (tileMap.getTileType(x/s, y/s) == TileType::BACKGROUND_CLIMBABLE)
    {
        tile = TileType::BACKGROUND_CLIMBABLE;
        dx *= 1-cof;
        dy *= 1-cof;
    }
    else if (tileMap.getTileType(x/s, y/s) == TileType::BACKGROUND_ZERO_G)
    {
        dy += (gravity/100) * frameLength;
    }
    else
    {
        dy +=  gravity * frameLength;
    }

    double speed = sqrt(pow(dx, 2) + pow(dy, 2));
    if (speed > maxSpeed)
    {
        dx *= maxSpeed / speed;
        dy *= maxSpeed / speed;
    }

    int width = (sprite -> getTexture()) -> getSize().x;
    int height = (sprite -> getTexture()) -> getSize().y;
    double r_x = width / 2;
    double r_y = height / 2;
    double biggest_r = (r_x > r_y) ? r_x : r_y;

    int spread = ceil((biggest_r + deltaT * ((abs(dx) > abs(dy)) ? abs(dx) : abs(dy))) / s);

    for (int pass = 1; pass <= 2; pass++)
    {

        double bestT = deltaT;
        direction bestD = direction::none;
        TileType bestType = TileType::SOLID;

        for(int i = x/s - spread; i < x/s + spread; i++)
        {
            for(int j = y/s - spread; j < y/s + spread; j++)
            {
                if (tileMap.getTileType(i, j) == TileType::SOLID
                    || tileMap.getTileType(i, j) == TileType::SOLID_BOUNCY
                    || tileMap.getTileType(i, j) == TileType::SOLID_HIGH_FRICTION
                    || tileMap.getTileType(i, j) == TileType::SOLID_LOW_FRICTION)
                {

                    if (dx > 0 && !horizCol)
                    {
                        double t = ((i * s) - (x + r_x)) / dx;
                        double yy = y + t*dy;
                        if (t >= iota && t < bestT && (yy+r_y) > (j*s) && (yy-r_y) < ((j+1)*s))
                        {
                            bestT = t;
                            bestD = direction::left;
                            bestType = tileMap.getTileType(i, j);
                        }
                    }

                    if (dx < 0 && !horizCol)
                    {
                        double t = ((x - r_x) - ((i+1) * s)) / (-dx);
                        double yy = y + t*dy;
                        if (t >= iota && t < bestT && (yy+r_y) > (j*s) && (yy-r_y) < ((j+1)*s))
                        {
                            bestT = t;
                            bestD = direction::right;
                            bestType = tileMap.getTileType(i, j);
                        }
                    }

                    if (dy > 0 && !vertCol)
                    {
                        double t = ((j * s) - (y + r_y)) / dy;
                        double xx = x + t*dx;
                        if (t >= iota && t < bestT && (xx+r_x) > (i*s) && (xx-r_x) < ((i+1)*s))
                        {
                            bestT = t;
                            bestD = direction::top;
                            bestType = tileMap.getTileType(i, j);
                        }
                    }

                    if (dy < 0 && !vertCol)
                    {
                        double t = ((y - r_y) - ((j+1) * s)) / (-dy);
                        double xx = x + t*dx;
                        if (t >= iota && t < bestT && (xx+r_x) > (i*s) && (xx-r_x) < ((i+1)*s))
                        {
                            bestT = t;
                            bestD = direction::bottom;
                            bestType = tileMap.getTileType(i, j);
                        }
                    }
                }
            }
        }

        if (bestT < deltaT && bestD != direction::none)
        {
            x += dx * bestT;
            y += dy * bestT;
            deltaT -= bestT;
            if (bestD == direction::left || bestD == direction::right)
            {
                horizCol = true;
                switch (bestType)
                {
                    case TileType::SOLID_BOUNCY:
                        dx *= -1;
                        break;
                    case TileType::SOLID_HIGH_FRICTION:
                        dy *= 1-cof*4;
                    case TileType::SOLID:
                    case TileType::SOLID_LOW_FRICTION:
                        dx *= -cor;
                    default: break;
                }

            }
            if (bestD == direction::top || bestD == direction::bottom)
            {
                vertCol = true;
                switch (bestType)
                {
                    case TileType::SOLID_BOUNCY:
                        dy *= -1;
                        break;
                    case TileType::SOLID_HIGH_FRICTION:
                        dx *= 1-cof*4;
                    case TileType::SOLID:
                        dx *= 1-cof;
                    case TileType::SOLID_LOW_FRICTION:
                        dy = 0;
                    default: break;
                }

                if (bestD == direction::top && (tile == TileType::BACKGROUND || tile == TileType::FOREGROUND_LIQUID))
                {
                    tile = TileType::SOLID;
                    if (bestType == TileType::SOLID_HIGH_FRICTION
                        || bestType == TileType::SOLID_LOW_FRICTION) tile = bestType;
                }

            }
        }

    }

    if (horizCol || vertCol) setPosition(x, y);

    if (!(horizCol && vertCol)) setPosition(x + dx*deltaT, y + dy*deltaT);

    if (direction_dx > 0)
    {
        sprite -> setTextureRect(IntRect(width, 0, -width, height));
    }
    else if (direction_dx < 0)
    {
        sprite -> setTextureRect(IntRect(0, 0, width, height));
    }


}

TileType Entity::onTile()
{
    return tile;
}

void Entity::processEntityCollisions(vector<Entity*> entities, double frameLength)
{

    /*

        Function regions_intersect(Byval a As Rectangle, Byval b As Rectangle) As Integer

            If a.x2 < a.x1 Then Swap a.x2, a.x1
            If a.y2 < a.y1 Then Swap a.y2, a.y1
            If b.x2 < b.x1 Then Swap b.x2, b.x1
            If b.y2 < b.y1 Then Swap b.y2, b.y1
            If a.x1 <= b.x2 And a.x2 >= b.x1 And a.y1 <= b.y2 And a.y2 >= b.y1 Then Return true
            Return false

        End Function

        Sub collide_blocks(Byref alpha As Block, Byref beta As Block, frame_length As Double)

            Dim alpha_region As Rectangle
            With alpha_region
                .x1 = alpha.x + IIf(alpha.dx > 0, -1, +1) * alpha.w/2
                .y1 = alpha.y + IIf(alpha.dy > 0, -1, +1) * alpha.h/2
                .x2 = alpha.x + IIf(alpha.dx < 0, -1, +1) * alpha.w/2 + alpha.dx * frame_length
                .y2 = alpha.y + IIf(alpha.dy < 0, -1, +1) * alpha.h/2 + alpha.dy * frame_length
            End With

            Dim beta_region As Rectangle
            With beta_region
                .x1 = beta.x + IIf(beta.dx > 0, -1, +1) * beta.w/2
                .y1 = beta.y + IIf(beta.dy > 0, -1, +1) * beta.h/2
                .x2 = beta.x + IIf(beta.dx < 0, -1, +1) * beta.w/2 + beta.dx * frame_length
                .y2 = beta.y + IIf(beta.dy < 0, -1, +1) * beta.h/2 + beta.dy * frame_length
            End With

            If regions_intersect(alpha_region, beta_region) Then

                Dim horiz_time As Double = 999
                Dim alpha_rx As Double = 0
                Dim beta_rx As Double = 0

                If alpha.x < beta.x Then
                    If Not (alpha.dx < 0 And beta.dx > 0) Then
                       alpha_rx = alpha.w / 2
                       beta_rx = -beta.w / 2
                    End If
                Else
                    If Not (alpha.dx > 0 And beta.dx < 0) Then
                       alpha_rx = -alpha.w / 2
                       beta_rx = beta.w / 2
                    End If
                End If

                If alpha_rx <> 0 And beta_rx <> 0 And alpha.dx - beta.dx <> 0 Then
                    horiz_time = (alpha.x + alpha_rx - beta.x - beta_rx) / (alpha.dx - beta.dx)
                    If horiz_time < iota Then horiz_time = 999
                End If

                Dim vert_time As Double = 999
                Dim alpha_ry As Double = 0
                Dim beta_ry As Double = 0

                If alpha.y < beta.y Then
                    If Not (alpha.dy < 0 And beta.dy > 0) Then
                       alpha_ry = alpha.h / 2
                       beta_ry = -beta.h / 2
                    End If
                Else
                    If Not (alpha.dy > 0 And beta.dy < 0) Then
                       alpha_ry = -alpha.h / 2
                       beta_ry = beta.h / 2
                    End If
                End If

                If alpha_ry <> 0 And beta_ry <> 0 And alpha.dy - beta.dy <> 0 Then
                    vert_time = (alpha.y + alpha_ry - beta.y - beta_ry) / (alpha.dy - beta.dy)
                    If vert_time < iota Then vert_time = 999
                End If

                If vert_time < frame_length Or horiz_time < frame_length Then

                    Dim m1 As Double = alpha.w * alpha.h
                    Dim m2 AS Double = beta.w * beta.h

                    If horiz_time < vert_time Then

                        Dim u1 As Double = alpha.dx
                        Dim u2 As Double = beta.dx
                        alpha.dx = (u1 * (m1 - m2) + 2 * m2 * u2) / (m1 + m2)
                        beta.dx = (u2 * (m2 - m1) + 2 * m1 * u1) / (m1 + m2)

                    Else

                        Dim u1 As Double = alpha.dy
                        Dim u2 As Double = beta.dy
                        alpha.dy = (u1 * (m1 - m2) + 2 * m2 * u2) / (m1 + m2)
                        beta.dy = (u2 * (m2 - m1) + 2 * m1 * u1) / (m1 + m2)

                    End If

                End If



            End If

        End Sub



        Sub repel_blocks(Byref alpha As Block, Byref beta As Block, frame_length As Double)


            Dim alpha_region As Rectangle
            With alpha_region
                .x1 = alpha.x - alpha.w/2
                .y1 = alpha.y - alpha.h/2
                .x2 = alpha.x + alpha.w/2
                .y2 = alpha.y + alpha.h/2
            End With

            Dim beta_region As Rectangle
            With beta_region
                .x1 = beta.x - beta.w/2
                .y1 = beta.y - beta.h/2
                .x2 = beta.x + beta.w/2
                .y2 = beta.y + beta.h/2
            End With

            If regions_intersect(alpha_region, beta_region) Then

                Dim u As Double = beta.x - alpha.x
                Dim v As Double = beta.y - alpha.y
                Dim d As Double = sqr(u^2 + v^2)

                u *= repulsion / d
                v *= repulsion / d

                alpha.dx -= u
                alpha.dy -= v

                beta.dx += u
                beta.dy += v


            End If

        End Sub

    */
}

#include <SFML/Graphics.hpp>

#include "tilemap.hpp"

#include <iostream>
#include <sstream>

#define str(x) dynamic_cast< std::ostringstream & >( \
       ( std::ostringstream() << std::dec << x ) ).str()

using namespace std;
using namespace sf;

TileMap::TileMap(int width, int height, int tileSize)
{

	_width = width;
	_height = height;
	_tileSize = tileSize;
    _frameCounter = 0;

	if (width > maxWidth) width = maxWidth;
	if (height > maxHeight) height = maxHeight;

	clear();

    int column = 0;
    int row = 1;
    for (int i = 0; i < 128; i++)
    {
        column++;
        if (column > 8)
        {
            column = 1;
            row++;
        }
        _tileTexture[i].loadFromFile("tiles/" + str(row) + "_" + str(column) + ".png");
        _tileSprite[i].setTexture(_tileTexture[i]);
        _tileSprite[i].setScale(0.5, 0.5);
    }

}

void TileMap::clear()
{

    _frameCounter = 0;

    for(auto i = 0; i < _width; i++)
    {
        for(auto j = 0; j < _height; j++)
        {
            _map[i][j] = 0;
            _mapUndo[i][j].tile = 0;
            _mapUndo[i][j].frame = -1;
        }
    }

}

int TileMap::getTile(int x, int y)
{
	if (x >= 0 && y >= 0 && x < _width && y < _height)
	{
		return _map[x][y];
	}
	else
	{
		return 0;
	}
}

void TileMap::setTile(int x, int y, int value)
{
	if (x >= 0 && y >= 0 && x < _width && y < _height)
	{
		if (_map[x][y] != value)
		{
            _mapUndo[x][y].tile = _map[x][y];
            _mapUndo[x][y].frame = _frameCounter;
		}
		_map[x][y] = value;
	}
}

void TileMap::drawBackground(RenderWindow & target)
{

    _frameCounter += 1;

    RectangleShape block;
	block.setSize(Vector2f(_tileSize, _tileSize));

    for(int i = -1; i <= _width; i++)
    {
        for(int j = -1; j <= _height; j++)
        {

            if (i < 0 || j < 0 || i == _width || j == _height)
            {
                block.setFillColor(Color(64, 0, 0));
                block.setPosition(_tileSize*i, _tileSize*j);
                target.draw(block);
            }
            else
            {
                int tileNo = _map[i][j] - 1;
                if (tileNo >= 0 && tileNo < 128)
                {
                    auto t = getTileType(i, j);
                    if (t == TileType::FOREGROUND_LIQUID)
                    {
                        _tileSprite[tileNo].setColor(Color(64, 64, 64, 255));
                        _tileSprite[tileNo].setPosition(_tileSize*i, _tileSize*j);
                        target.draw(_tileSprite[tileNo]);
                    }
                    else if (t != TileType::FOREGROUND)
                    {
                        _tileSprite[tileNo].setColor(Color(255, 255, 255, 255));
                        _tileSprite[tileNo].setPosition(_tileSize*i, _tileSize*j);
                        target.draw(_tileSprite[tileNo]);
                    }
                }
            }
        }
    }

}


void TileMap::drawForeground(RenderWindow & target)
{

    _frameCounter += 1;

    RectangleShape block;
	block.setSize(Vector2f(_tileSize, _tileSize));

    for(int i = -1; i <= _width; i++)
    {
        for(int j = -1; j <= _height; j++)
        {

            if (i < 0 || j < 0 || i == _width || j == _height)
            {
                block.setFillColor(Color(64, 0, 0));
                block.setPosition(_tileSize*i, _tileSize*j);
                target.draw(block);
            }
            else
            {
                int tileNo = _map[i][j] - 1;
                if (tileNo >= 0 && tileNo < 128)
                {
                    auto t = getTileType(i, j);
                    if (t == TileType::FOREGROUND)
                    {
                        _tileSprite[tileNo].setColor(Color(255, 255, 255, 255));
                        _tileSprite[tileNo].setPosition(_tileSize*i, _tileSize*j);
                        target.draw(_tileSprite[tileNo]);
                    }
                    else if (t == TileType::FOREGROUND_ALPHA)
                    {
                        _tileSprite[tileNo].setColor(Color(255, 255, 255, 192));
                        _tileSprite[tileNo].setPosition(_tileSize*i, _tileSize*j);
                        target.draw(_tileSprite[tileNo]);
                    }
                    else if (t == TileType::FOREGROUND_LIQUID)
                    {
                        _tileSprite[tileNo].setColor(Color(255, 255, 255, 128));
                        _tileSprite[tileNo].setPosition(_tileSize*i, _tileSize*j);
                        target.draw(_tileSprite[tileNo]);
                    }
                }
            }
        }
    }

}


void TileMap::drawHighlight(RenderWindow & target, int startX, int startY, int endX, int endY, Color color)
{

	RectangleShape block;
	block.setSize(Vector2f(_tileSize, _tileSize));
	block.setFillColor(color);

	if (startX > endX) swap (startX, endX);
	if (startY > endY) swap (startY, endY);

    for(int i = startX; i <= endX; i++)
    {
        for(int j = startY; j <= endY; j++)
        {
        	if (i >= 0 && j >= 0 && i < _width && j < _height)
        	{
				block.setPosition(_tileSize*i, _tileSize*j);
				target.draw(block);
			}
		}
	}

}

void TileMap::drawCursor(RenderWindow & target, int x, int y)
{

    RectangleShape block;
    block.setSize(Vector2f(_tileSize, _tileSize));
    block.setFillColor(Color(0, 255, 0, 128));

    if (x >= 0 && y >= 0 && x < _width && y < _height)
    {
        block.setPosition(_tileSize*x, _tileSize*y);
        target.draw(block);
    }

}

void TileMap::drawTileOverlay(RenderWindow & target, int currentTile, int add)
{

    RectangleShape block;
    block.setSize(Vector2f(target.getSize().x, target.getSize().y));
    block.setPosition(0, 0);
    block.setFillColor(Color(0, 0, 0, 192));
    target.draw(block);

    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            int tileNo = i + j * 8 + add;

            if (tileNo == currentTile - 1)
            {
                block.setSize(Vector2f(_tileSize + 16, _tileSize + 16));
                block.setPosition((_tileSize + 32) * i + 255 + 8 , (_tileSize + 32) * j + 8);
                block.setFillColor(Color(255, 255, 255, 192));
                target.draw(block);
                _tileSprite[tileNo].setColor(Color(255, 255, 255, 255));
            }
            else
            {
                _tileSprite[tileNo].setColor(Color(192, 192, 192, 128));
            }

            _tileSprite[tileNo].setPosition((_tileSize + 32) * i + 255 + 16, (_tileSize + 32) * j + 16);
            target.draw(_tileSprite[tileNo]);
        }
    }
}

int TileMap::width()
{
	return _width;
}

int TileMap::height()
{
	return _height;
}

int TileMap::centreX()
{
	return _width / 2;
}

int TileMap::centreY()
{
	return _height / 2;
}

int TileMap::tileSize()
{
	return _tileSize;
}

TileType TileMap::getTileType(int i, int j)
{
    auto t = getTile(i, j);
    if (t < 1 || t > 128) return TileType::BACKGROUND;

    int row = (t-1) / 8;
    if (row < 0 || row > 15) return TileType::BACKGROUND;

    return _tileRowTypes[row];

}

TileType TileMap::getRowType(int row)
{
    if (row < 0 || row > 15) return TileType::BACKGROUND;
    return _tileRowTypes[row];
}

long TileMap::lastEditFrame()
{
    long lastFrame = -1;

    for(auto i = 0; i < _width; i++)
    {
        for(auto j = 0; j < _height; j++)
        {
            if (_mapUndo[i][j].frame > lastFrame)
            {
                lastFrame = _mapUndo[i][j].frame;
            }
        }
    }

    return lastFrame;

}

void TileMap::applyUndo(long frame)
{
    if (frame == -1) return;

    for(auto i = 0; i < _width; i++)
    {
        for(auto j = 0; j < _height; j++)
        {
            if (_mapUndo[i][j].frame == frame)
            {
                _map[i][j] = _mapUndo[i][j].tile;
                _mapUndo[i][j].tile = 0;
                _mapUndo[i][j].frame = -1;
            }
        }
    }
}

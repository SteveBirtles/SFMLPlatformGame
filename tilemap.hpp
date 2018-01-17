#pragma once

#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

struct Undo
{
    int tile;
    long frame;
};

enum class TileType {
    BACKGROUND,
    BACKGROUND_CLIMBABLE,
    BACKGROUND_ZERO_G,
    SOLID,
    SOLID_LOW_FRICTION,
    SOLID_HIGH_FRICTION,
    SOLID_BOUNCY,
    FOREGROUND,
    FOREGROUND_ALPHA,
    FOREGROUND_LIQUID
};

class TileMap
{

	private:

		const static int maxWidth = 255;
		const static int maxHeight = 255;

		int _map[maxWidth][maxHeight];
		Undo _mapUndo[maxWidth][maxHeight];
		int _width;
		int _height;
		int _tileSize;
		long _frameCounter;

		Sprite _tileSprite[128];
		Texture _tileTexture[128];

        TileType _tileRowTypes[16] =
        {
            TileType::BACKGROUND, TileType::BACKGROUND, TileType::BACKGROUND,
            TileType::BACKGROUND_CLIMBABLE,
            TileType::BACKGROUND_ZERO_G,
            TileType::SOLID, TileType::SOLID, TileType::SOLID,
            TileType::SOLID_LOW_FRICTION,
            TileType::SOLID_HIGH_FRICTION,
            TileType::SOLID_BOUNCY,
            TileType::FOREGROUND, TileType::FOREGROUND, TileType::FOREGROUND,
            TileType::FOREGROUND_ALPHA,
            TileType::FOREGROUND_LIQUID
        };

	public:

		TileMap(int width, int height, int tileSize);	// CONSTRUCTOR

		void clear();

		int getTile(int x, int y);
		TileType getTileType(int x, int y);
		TileType getRowType(int row);

		void setTile(int x, int y, int value);

        void drawBackground(RenderWindow & target);
		void drawForeground(RenderWindow & target);
		void drawCursor(RenderWindow & target, int x, int y);
		void drawHighlight(RenderWindow & target, int startX, int startY, int endX, int endY, Color color);
		void drawTileOverlay(RenderWindow & target, int currentTile, int add);

        void applyUndo(long frame);

		int width();
		int height();
		int centreX();
		int centreY();
		int tileSize();

		void LoadFromFile(string filename);	 // See tilefile.cpp
		void SaveToFile(string filename);	 // See tilefile.cpp

		long lastEditFrame();

	};

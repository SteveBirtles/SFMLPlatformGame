#include <iostream>
#include <fstream>

#include <SFML/Graphics.hpp>

#include "tilemap.hpp"

using namespace std;
using namespace sf;

void TileMap::LoadFromFile(string filename)
{
	ifstream levelfile(filename);

	int buffersize = sizeof(int);

	if (buffersize != 4)
	{
		cout << "Can't load files on this system architecture!";
		return;
	}
	
	char* buffer = new char[buffersize];

	if(levelfile)
	{

		levelfile.read(buffer, buffersize);
		int loadWidth = *buffer;

		levelfile.read(buffer, buffersize);
		int loadHeight = *buffer;

		if (loadWidth > 0 && loadWidth <= maxWidth && loadHeight > 0 && loadHeight <= maxHeight)
		{
			
			_width = loadWidth;
			_height = loadHeight;
			clear();

			for (int i = 0; i < _width; i++)
			{
				for (int j = 0; j < _height; j++)
				{
					if (levelfile.eof()) break;
					levelfile.read(buffer, buffersize);						
					_map[i][j] = *buffer;
				}
			}

		}

		levelfile.close();

	}
	else
	{
		clear();
	}
}

void TileMap::SaveToFile(string filename)
{
	ofstream levelfile(filename);

	int buffersize = sizeof(int);
	
	if (buffersize != 4)
	{
		cout << "Can't save files on this system architecture!";
		return;
	}
	
	char* buffer = new char[buffersize];

	if (levelfile.is_open())
	{
		*buffer = _width;
		levelfile.write(buffer, buffersize);
		*buffer = _height;
		levelfile.write(buffer, buffersize);
	
		for (int i = 0; i < _width; i++)
		{
			for (int j = 0; j < _height; j++)
			{			
				*buffer = _map[i][j];		
				levelfile.write(buffer, buffersize);
			}
		}

	}

	levelfile.close();

}

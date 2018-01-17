#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>

using namespace std;
using namespace sf;

#define str(x) dynamic_cast< std::ostringstream & >( \
       ( std::ostringstream() << std::dec << x ) ).str()

#include "camera.hpp"
#include "tilemap.hpp"

int windowWidth = 1024;
int windowHeight = 768;

int currentTile = 1;
int replaceTile = 0;

int mouseX = 0;
int mouseY = 0;
int mouseI = 0;
int mouseJ = 0;

int selectionStartI = 0;
int selectionStartJ = 0;
int selectionEndI = 0;
int selectionEndJ = 0;
bool resetSelection = true;

int clipboard[255][255];
int clipboardWidth = 0;
int clipboardHeight = 0;

bool undoPause = false;

bool mouseInWindow(RenderWindow & window, Camera & camera, TileMap & tileMap)
{
    mouseX = Mouse::getPosition(window).x;
    mouseY = Mouse::getPosition(window).y;

    if (mouseX > 0 && mouseY > 0 && mouseX < window.getSize().x && mouseY < window.getSize().y)
    {
        float mouseU = ((mouseX -  windowWidth/2) * camera.scale());
        float mouseV = ((mouseY - windowHeight/2) * camera.scale());
        mouseI = (cos(camera.radians()) * mouseU - sin(camera.radians()) * mouseV + camera.x()) / tileMap.tileSize();
        mouseJ = (sin(camera.radians()) * mouseU + cos(camera.radians()) * mouseV + camera.y()) / tileMap.tileSize();
        return true;
    }

   return false;
}


int main()
{

    string levelName = "";
    cout << "Please type in a level name to load or create:\n(n.b. file extension will be added automatically)\n\n";
    cin >> levelName;
    cout << "\n";

    auto desktop = VideoMode::getDesktopMode();

    RenderWindow window(VideoMode(windowWidth, windowHeight, desktop.bitsPerPixel), "SFML Level Editor", 0);
    window.setPosition(Vector2i((desktop.width - windowWidth) / 2, (desktop.height - windowHeight) / 2));

    Font font;
    font.loadFromFile("fonts/fabiolo-light.ttf");

    Text text;
    text.setFont(font);
    text.setColor(Color(255,255,255));
    text.setCharacterSize(45);

    TileMap tileMap(100, 100, 64);
    tileMap.LoadFromFile("levels/" + levelName + ".dat");

    map <TileType, string> tileTypeNames;

    tileTypeNames[TileType::BACKGROUND] = "Background";
    tileTypeNames[TileType::BACKGROUND_CLIMBABLE] = "Climbable";
    tileTypeNames[TileType::BACKGROUND_ZERO_G] = "Zero G";
    tileTypeNames[TileType::SOLID] = "Solid";
    tileTypeNames[TileType::SOLID_LOW_FRICTION] = "Low Friction";
    tileTypeNames[TileType::SOLID_HIGH_FRICTION] = "High Friction";
    tileTypeNames[TileType::SOLID_BOUNCY] = "Bouncy";
    tileTypeNames[TileType::FOREGROUND] = "Foreground";
    tileTypeNames[TileType::FOREGROUND_ALPHA] = "Alpha";
    tileTypeNames[TileType::FOREGROUND_LIQUID] = "Liquid";

    Camera camera;
    camera.setCentre(6 * tileMap.tileSize(), 4 * tileMap.tileSize());

    Clock gameClock;
    double frameLength = 0;

    while (window.isOpen())
    {

        double frameStart = gameClock.getElapsedTime().asSeconds();

        Event eventcatcher;
        while (window.pollEvent(eventcatcher))
        {
            if (eventcatcher.type == Event::Closed)
            {
                window.close();
                break;
            }
        }

        if (mouseInWindow(window, camera, tileMap))
        {

            if (!Keyboard::isKeyPressed(Keyboard::LControl))
            {

                if (Keyboard::isKeyPressed(Keyboard::W)) camera.moveUp(500 * frameLength);
                if (Keyboard::isKeyPressed(Keyboard::S)) camera.moveDown(500 * frameLength);
                if (Keyboard::isKeyPressed(Keyboard::A)) camera.moveLeft(500 * frameLength);
                if (Keyboard::isKeyPressed(Keyboard::D)) camera.moveRight(500 * frameLength);

                if (Keyboard::isKeyPressed(Keyboard::R)) camera.zoomOut(1 + frameLength);
                if (Keyboard::isKeyPressed(Keyboard::F)) camera.zoomIn(1 + frameLength);
                if (Keyboard::isKeyPressed(Keyboard::Q)) camera.rotate(-90 * frameLength);
                if (Keyboard::isKeyPressed(Keyboard::E)) camera.rotate(90 * frameLength);

                if (Keyboard::isKeyPressed(Keyboard::Escape))
                {
                    selectionEndI = -1;
                    selectionEndJ = -1;
                    resetSelection = true;
                    clipboardWidth = 0;
                    clipboardHeight = 0;
                }
            }


            if (Keyboard::isKeyPressed(Keyboard::LAlt) || Keyboard::isKeyPressed(Keyboard::RAlt))
            {
                int squareX = (mouseX- 256) / 96 ;
                int squareY = mouseY / 96;

                if (squareX < 0) squareX = 0;
                if (squareX > 7) squareX = 7;

                if (squareY < 0) squareY = 0;
                if (squareY > 7) squareY = 7;

                currentTile = squareX + squareY * 8 + 1;

                if (Keyboard::isKeyPressed(Keyboard::RAlt)) currentTile += 64;

            }

            int processSelection = 0;


            if(selectionEndI != -1 && selectionEndJ != -1)
            {
                if (Keyboard::isKeyPressed(Keyboard::LControl) && Keyboard::isKeyPressed(Keyboard::F)) processSelection = 1;
                if (Keyboard::isKeyPressed(Keyboard::LControl) && Keyboard::isKeyPressed(Keyboard::Delete)) processSelection = 2;
                if (Keyboard::isKeyPressed(Keyboard::LControl) && Keyboard::isKeyPressed(Keyboard::X)) processSelection = 3;
                if (Keyboard::isKeyPressed(Keyboard::LControl) && Keyboard::isKeyPressed(Keyboard::C)) processSelection = 4;
                if (Keyboard::isKeyPressed(Keyboard::LControl) && Keyboard::isKeyPressed(Keyboard::P))
                {
                    replaceTile = tileMap.getTile(mouseI, mouseJ);
                    processSelection = 5;
                }
            }

            if (Keyboard::isKeyPressed(Keyboard::LControl) && Keyboard::isKeyPressed(Keyboard::N)) tileMap.clear();
            if (Keyboard::isKeyPressed(Keyboard::LControl) && Keyboard::isKeyPressed(Keyboard::S))
            {
                tileMap.LoadFromFile("levels/" + levelName + ".dat");
            }

            if (Keyboard::isKeyPressed(Keyboard::LControl) && Keyboard::isKeyPressed(Keyboard::L))
            {
                tileMap.LoadFromFile("levels/" + levelName + ".dat");
            }


            if (Keyboard::isKeyPressed(Keyboard::LControl) && Keyboard::isKeyPressed(Keyboard::Z))
            {
                if (!undoPause)
                {
                    tileMap.applyUndo(tileMap.lastEditFrame());
                    undoPause = true;
                }
            }
            else
            {
                undoPause = false;
            }


            if (Keyboard::isKeyPressed(Keyboard::LControl) && Keyboard::isKeyPressed(Keyboard::A))
            {
                selectionStartI = 0;
                selectionStartJ = 0;
                selectionEndI = tileMap.width() - 1;
                selectionEndJ = tileMap.height() - 1;
            }

            if (Keyboard::isKeyPressed(Keyboard::LControl) && Keyboard::isKeyPressed(Keyboard::Q))
            {
                window.close();
                break;
            }

            if (processSelection > 0)
            {

                int processStartI = selectionStartI;
                int processEndI = selectionEndI;
                if (processStartI > processEndI) swap (processStartI, processEndI);

                int processStartJ = selectionStartJ;
                int processEndJ = selectionEndJ;
                if (processStartJ > processEndJ) swap (processStartJ, processEndJ);

                if (processSelection == 3 || processSelection == 4)
                {
                    clipboardWidth = processEndI - processStartI + 1;
                    clipboardHeight = processEndJ - processStartJ + 1;
                }

                for(int i = processStartI; i <= processEndI; i++)
                {
                    for(int j = processStartJ; j <= processEndJ; j++)
                    {
                        switch(processSelection)
                        {
                            case 1:
                                tileMap.setTile(i, j, currentTile);
                                break;
                            case 2:
                                tileMap.setTile(i, j, 0);
                                break;
                            case 3:
                                clipboard[i - processStartI][j - processStartJ] = tileMap.getTile(i, j);
                                tileMap.setTile(i, j, 0);
                                break;
                            case 4:
                                clipboard[i - processStartI][j - processStartJ] = tileMap.getTile(i, j);
                                break;
                            case 5:
                                if (tileMap.getTile(i, j) == replaceTile) tileMap.setTile(i, j, currentTile);
                                break;
                        }
                    }
                }
            }
            else
            {
                if (Keyboard::isKeyPressed(Keyboard::LControl))
                {
                    for(int i = 0; i < clipboardWidth; i++)
                    {
                        for(int j = 0; j < clipboardHeight; j++)
                        {
                            if (Keyboard::isKeyPressed(Keyboard::V))
                                tileMap.setTile(mouseI + i, mouseJ + j, clipboard[i][j]);

                            else if (Keyboard::isKeyPressed(Keyboard::R))
                                tileMap.setTile(mouseI + clipboardHeight - j - 1, mouseJ + i, clipboard[i][j]);

                            else if (Keyboard::isKeyPressed(Keyboard::T))
                                tileMap.setTile(mouseI + clipboardWidth - i - 1, mouseJ + clipboardHeight - j - 1, clipboard[i][j]);

                            else if (Keyboard::isKeyPressed(Keyboard::Y))
                                tileMap.setTile(mouseI + j, mouseJ + clipboardWidth - i - 1, clipboard[i][j]);

                            else if (Keyboard::isKeyPressed(Keyboard::D))
                                tileMap.setTile(mouseI + i, mouseJ + clipboardHeight - j - 1, clipboard[i][j]);

                            else if (Keyboard::isKeyPressed(Keyboard::F))
                                tileMap.setTile(mouseI + clipboardWidth - i - 1, mouseJ + j, clipboard[i][j]);

                            else if (Keyboard::isKeyPressed(Keyboard::G))
                                tileMap.setTile(mouseI + j, mouseJ + i, clipboard[i][j]);

                            else if (Keyboard::isKeyPressed(Keyboard::H))
                                tileMap.setTile(mouseI + clipboardHeight - j - 1, mouseJ + clipboardWidth - i - 1, clipboard[i][j]);

                        }
                    }
                }
                else if (Keyboard::isKeyPressed(Keyboard::LShift))
                {
                   if (Mouse::isButtonPressed(Mouse::Left))
                    {
                        if (resetSelection == true)
                        {
                            selectionStartI = mouseI;
                            selectionStartJ = mouseJ;
                            resetSelection = false;
                        }

                        selectionEndI = mouseI;
                        selectionEndJ = mouseJ;
                    }
                }
                else
                {
                    if (Mouse::isButtonPressed(Mouse::Left)) tileMap.setTile(mouseI, mouseJ, currentTile);
                    if (Mouse::isButtonPressed(Mouse::Right)) tileMap.setTile(mouseI, mouseJ, 0);
                    if (Mouse::isButtonPressed(Mouse::Middle)) currentTile = tileMap.getTile(mouseI, mouseJ);

                    if (selectionStartI == selectionEndI && selectionStartJ == selectionEndJ)
                    {
                        selectionEndI = -1;
                        selectionEndJ = -1;
                    }
                }
            }

            if (!Mouse::isButtonPressed(Mouse::Left) && selectionStartI == selectionEndI && selectionStartJ == selectionEndJ)
            {
                selectionEndI = -1;
                selectionEndJ = -1;
            }

            if (!Mouse::isButtonPressed(Mouse::Left)) resetSelection = true;

        }

        window.clear();
        window.setView(camera.view(windowWidth, windowHeight));

        tileMap.drawForeground(window);
        tileMap.drawBackground(window);

        if (selectionEndI != -1 && selectionEndJ != -1)
        {
            tileMap.drawHighlight(window, selectionStartI, selectionStartJ, selectionEndI, selectionEndJ, Color(0, 0, 255, 128));
        }

        if (clipboardWidth > 0 && clipboardHeight > 0)
        {
            tileMap.drawHighlight(window, mouseI, mouseJ, mouseI + clipboardWidth - 1, mouseJ + clipboardHeight - 1, Color(0, 255, 255, 128));
            tileMap.drawHighlight(window, mouseI, mouseJ, mouseI + clipboardHeight - 1, mouseJ + clipboardWidth - 1, Color(0, 255, 255, 64));
        }

        tileMap.drawCursor(window, mouseI, mouseJ);

        if (Keyboard::isKeyPressed(Keyboard::RAlt))
        {
            window.setView(camera.overlayView(windowWidth, windowHeight));
            for (int i = 0; i < 8; i++)
            {
                text.setPosition(32, (tileMap.tileSize() + 32) * i + 16);
                text.setString(tileTypeNames[tileMap.getRowType(i+8)]);
                window.draw(text);
            }
            tileMap.drawTileOverlay(window, currentTile, 64);
        }
        else if (Keyboard::isKeyPressed(Keyboard::LAlt))
        {
            window.setView(camera.overlayView(windowWidth, windowHeight));
            for (int i = 0; i < 8; i++)
            {
                text.setPosition(32, (tileMap.tileSize() + 32) * i + 16);
                text.setString(tileTypeNames[tileMap.getRowType(i)]);
                window.draw(text);
            }
            tileMap.drawTileOverlay(window, currentTile, 0);
        }

        window.display();

        while (gameClock.getElapsedTime().asSeconds() - frameStart < 0.016667);
        frameLength = gameClock.getElapsedTime().asSeconds() - frameStart;

    }

    tileMap.SaveToFile("levels/" + levelName + ".dat");

    return 0;
}

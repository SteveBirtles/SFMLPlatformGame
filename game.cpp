#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <cmath>

using namespace std;
using namespace sf;

#define str(x) dynamic_cast< std::ostringstream & >( \
       ( std::ostringstream() << std::dec << x ) ).str()

#include "camera.hpp"
#include "tilemap.hpp"
#include "entity.hpp"

int windowWidth = 1024;
int windowHeight = 768;

int currentTile = 1;

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

bool mouseInWindow(RenderWindow & window, Camera & camera, TileMap & tileMap)
{
    mouseX = Mouse::getPosition(window).x;
    mouseY = Mouse::getPosition(window).y;

    if (mouseX > 0 && mouseY > 0 && mouseX < window.getSize().x && mouseY < window.getSize().y)
    {
        double mouseU = ((mouseX -  windowWidth/2) * camera.scale());
        double mouseV = ((mouseY - windowHeight/2) * camera.scale());
        mouseI = (cos(camera.radians()) * mouseU - sin(camera.radians()) * mouseV + camera.x()) / tileMap.tileSize();
        mouseJ = (sin(camera.radians()) * mouseU + cos(camera.radians()) * mouseV + camera.y()) / tileMap.tileSize();
        return true;
    }

   return false;
}


int main()
{

    string levelName = "";
    cout << "Please type in a level name to play:\n(n.b. file extension will be added automatically)\n\n";
    cin >> levelName;
    cout << "\n";

    auto desktop = VideoMode::getDesktopMode();

    RenderWindow window(VideoMode(windowWidth, windowHeight, desktop.bitsPerPixel), "Platform Game", 0);
    window.setPosition(Vector2i((desktop.width - windowWidth) / 2, (desktop.height - windowHeight) / 2));

    TileMap tileMap(100, 100, 64);
    tileMap.LoadFromFile("levels/" + levelName + ".dat");

    Camera camera;

    Clock gameClock;
    double frameLength = 0;

    /* DECLARATIONS */

    Texture EntityTexture[4];
    EntityTexture[0].loadFromFile("sprites/fry.png");
    EntityTexture[1].loadFromFile("sprites/bender.png");
    EntityTexture[2].loadFromFile("sprites/nibbler.png");
    EntityTexture[3].loadFromFile("sprites/nixon.png");

    vector<Entity*> entities;

    auto myEntity = new Entity(EntityTexture[0]);
    myEntity -> setPosition(300, 300);
    entities.push_back(myEntity);

    for (int i = 0; i < 100; i++)
    {
        auto e = new Entity(EntityTexture[(i % 3) + 1]);
        e -> setPosition(i*100+100, 200);
        entities.push_back(e);
    }

    bool jumpPause = false;


    /* ------------ */

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

            if (Keyboard::isKeyPressed(Keyboard::Escape))
            {
                window.close();
                break;
            }

            /* INPUTS */


            bool isgrounded = false;
            bool canmovefast = false;
            bool onsoftground = false;
            bool canclimb = false;
            bool underwater = false;

            switch (myEntity -> onTile())
            {
                case TileType::FOREGROUND_LIQUID:
                    underwater = true;
                    break;
                case TileType::SOLID:
                    canmovefast = true;
                case TileType::SOLID_BOUNCY:
                case TileType::SOLID_LOW_FRICTION:
                    isgrounded = true;
                    break;
                case TileType::SOLID_HIGH_FRICTION:
                    isgrounded = true;
                    onsoftground = true;
                    break;
                case TileType::BACKGROUND_CLIMBABLE:
                    canclimb = true;
                    canmovefast = true;
                    break;
                default:
                    break;
            }


            if (Keyboard::isKeyPressed(Keyboard::Up))
            {
                if (isgrounded && !jumpPause)
                {
                    myEntity -> setVelocity(myEntity -> getDX(), myEntity -> getDY() - (onsoftground ? 600 : 1000));
                    jumpPause = true;
                }
                else if (underwater || canclimb)
                {
                    myEntity -> setVelocity(myEntity -> getDX(), myEntity -> getDY() - 2500 * frameLength);
                }
            }
            else
            {
                jumpPause = false;
            }


            if (Keyboard::isKeyPressed(Keyboard::Down))
            {
                if (underwater || canclimb)
                {
                    myEntity -> setVelocity(myEntity -> getDX(), myEntity -> getDY() + 2500 * frameLength);
                }
            }

            if (Keyboard::isKeyPressed(Keyboard::Left))
            {
                myEntity -> setVelocity(myEntity -> getDX() - (canmovefast ? 2500 : 500) * frameLength, myEntity -> getDY());
                myEntity -> updateDirection(-1);
            }

            if (Keyboard::isKeyPressed(Keyboard::Right))
            {
                myEntity -> setVelocity(myEntity -> getDX() + (canmovefast ? 2500 : 500) * frameLength, myEntity -> getDY());
                myEntity -> updateDirection(+1);
            }

            /* ------ */
        }

        /* PROCESSES */

        Entity::processEntityCollisions(entities, frameLength);

        for(auto e : entities)
            e -> updatePosition(frameLength, tileMap);

        camera.setCentre(myEntity -> getX(), myEntity -> getY());


        /* --------- */

        window.clear();
        window.setView(camera.view(windowWidth, windowHeight));

        tileMap.drawBackground(window);

        /* OUPUTS */

        for(auto e : entities)
            window.draw(e -> getSprite());

        tileMap.drawForeground(window);

        /* ------ */

        window.display();

        while (gameClock.getElapsedTime().asSeconds() - frameStart < 0.016667);
        frameLength = gameClock.getElapsedTime().asSeconds() - frameStart;
        if (frameLength > 0.025) frameLength = 0.025;

    }

    return 0;
}

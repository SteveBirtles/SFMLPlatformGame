#!/bin/bash
g++ -std=c++11 -o Game.exe tilefile.cpp tilemap.cpp camera.cpp entity.cpp game.cpp -lsfml-graphics-d -lsfml-window-d -lsfml-system-d -DSFML_STATIC
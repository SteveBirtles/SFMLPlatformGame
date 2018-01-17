#!/bin/bash
g++ -std=c++11 -o Editor.exe tilefile.cpp tilemap.cpp camera.cpp entity.cpp editor.cpp -lsfml-graphics-d -lsfml-window-d -lsfml-system-d -DSFML_STATIC
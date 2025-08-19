# Makefile for SFML 3.0.0 static linking on Windows (MinGW)

all: compile link

compile:
	g++ -c main.cpp -I"C:\libraries\SFML-3.0.0\include" -DSFML_STATIC

link:
	g++ main.o -o main.exe -L"C:\libraries\SFML-3.0.0\lib" \
    -lsfml-graphics-s -lsfml-window-s -lsfml-system-s \
    -lopengl32 -lfreetype -lwinmm -lgdi32 -mwindows


clean:
	del main.exe main.o

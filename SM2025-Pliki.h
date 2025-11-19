// funkcje do operacji na plikach
#ifndef SM2025_PLIKI_H_INCLUDED
#define SM2025_PLIKI_H_INCLUDED

#include <stdio.h>
#include <math.h>
#include <fstream>
#include <SDL2/SDL.h>
struct ByteRun{
    int* tab;
    int len;
    ByteRun(int* t, int l) : tab(t), len(l) {}
};

struct ByteRunColors{
    ByteRun* rtab;
    ByteRun* gtab;
    ByteRun* btab;
    ByteRunColors(ByteRun* r,ByteRun* g,ByteRun* b) : rtab(r), gtab(g), btab(b){}
};
ByteRun* ByteRunKompresja(int wejscie[], int dlugosc) ;
ByteRun* ByteRunDekompresja(int wejscie[], int dlugosc) ;
ByteRunColors kompresjaObrazu(SDL_Color colors[], int len);
SDL_Color* dekompresjObrazu(ByteRunColors* colors);
void zapisz(ByteRunColors* colors) ;
void wczytaj(ByteRunColors* colors);
#endif // SM2025_PLIKI_H_INCLUDED

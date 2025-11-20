// funkcje do operacji na plikach
#ifndef SM2025_PLIKI_H_INCLUDED
#define SM2025_PLIKI_H_INCLUDED

#include <stdio.h>
#include <math.h>
#include <fstream>
#include <SDL2/SDL.h>
struct ByteRun {
    int16_t* tab;
    int len;
    ByteRun(int16_t* t, int l) : tab(t), len(l) {}
    ~ByteRun() { delete[] tab; }
};

struct ByteRunColors{
    ByteRun* rtab;
    ByteRun* gtab;
    ByteRun* btab;
    ByteRunColors(ByteRun* r,ByteRun* g,ByteRun* b) : rtab(r), gtab(g), btab(b){}
};
ByteRun* ByteRunKompresja(int wejscie[], int dlugosc) ;
ByteRun* ByteRunDekompresja(int wejscie[], int dlugosc) ;
ByteRunColors kompresjaObrazu(Kolor colors[], int len);
Kolor* dekompresjObrazu(ByteRunColors* colors);
void zapisz(ByteRunColors* colors) ;
void wczytaj(ByteRunColors* colors);
#endif // SM2025_PLIKI_H_INCLUDED

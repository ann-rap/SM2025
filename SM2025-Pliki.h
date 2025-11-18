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
ByteRun* ByteRunKompresja(int wejscie[], int dlugosc) ;
ByteRun* ByteRunDekompresja(int wejscie[], int dlugosc) ;

#endif // SM2025_PLIKI_H_INCLUDED

// funkcje do operacji na plikach
#ifndef SM2025_PLIKI_H_INCLUDED
#define SM2025_PLIKI_H_INCLUDED

#include <stdio.h>
#include <math.h>
#include <fstream>
#include <SDL2/SDL.h>

/*
Funkcje i struktury do ByteRun (projekt 6)
*/
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


/*
Funkcje i struktury do RLE (projekt 7)

*/

struct RLE {
    int16_t* tab;
    int len;
    RLE(int16_t* t, int l) : tab(t), len(l) {}
    ~RLE() { delete[] tab; }
};

struct RLEColors {
    RLE* rtab;
    RLE* gtab;
    RLE* btab;
    RLEColors(RLE* r, RLE* g, RLE* b) : rtab(r), gtab(g), btab(b) {}
};

RLE* RLEKompresja(int wejscie[], int dlugosc);
RLE* RLEDekompresja(int16_t wejscie[], int dlugosc);
RLEColors kompresjaObrazu_RLE(Kolor colors[], int len);
Kolor* dekompresjaObrazu_RLE(RLEColors* colors);
void zapisz_RLE(RLEColors* colors, const char* filename);
void wczytaj_RLE(RLEColors* colors, const char* filename);
#endif // SM2025_PLIKI_H_INCLUDED

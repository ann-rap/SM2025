// funkcje do operacji na plikach
#ifndef SM2025_PLIKI_H_INCLUDED
#define SM2025_PLIKI_H_INCLUDED

#include <stdio.h>
#include <math.h>
#include <fstream>
#include <vector>
#include <SDL2/SDL.h>


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
void zapiszPojedynczyRLE(std::ofstream& out, RLE* rle);
void wczytajPojedynczyRLE(std::ifstream& in, RLE* rle);


void zapiszRLEdoStrumienia(ofstream& plik, RLEColors* rle);

#endif // SM2025_PLIKI_H_INCLUDED

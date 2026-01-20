// funkcje do redukcji kolor�w i tworzenia palet
#ifndef SM2025_PALETA_H_INCLUDED
#define SM2025_PALETA_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <iomanip>

using namespace std;
const int rozmiarBloku=16;
// Struktura do zwracania wynikow
struct WynikStruct {
    SDL_Color paleta[64];      // Paleta kolor�w
    Uint8 indeksy[64000];      //macierz zawierajaca indeksy z palety kolor�w
};

struct macierz {
    float dct[rozmiarBloku][rozmiarBloku];
    Uint8 dane[rozmiarBloku][rozmiarBloku];
};


struct YCbCr{
    float y;
    float cb;
    float cr;
};

struct Kolor{
    float c1;
    float c2;
    float c3;
};

struct DaneKompresji {
    Uint8* Y;
    Uint8* Cb;
    Uint8* Cr;
};


void filtrujYCbCr_Typ3(bool isGray);
void odfiltrujYCbCr_Typ3();


void wyswietlDane(macierz blok);
void wyswietlDCT(macierz blok);
macierz dct(Uint8 wartosci[rozmiarBloku][rozmiarBloku]);
macierz idct(float DCT[rozmiarBloku][rozmiarBloku]);
 void kompresjaDCT();
// Funkcje do kompresji DCT z zygzakowaniem
void zigzagCollect(float dct[rozmiarBloku][rozmiarBloku], float output[256]);
void zigzagReconstruct(float zigzag[256], float dct[rozmiarBloku][rozmiarBloku]);

void subsample420_YCbCr(int width, int height);

void setPixelByStruct(int x, int y, Kolor kolor);
float normalize(float part);
Kolor normalizeAll(float r,float g,float b);

void setYCbCr(int xx, int yy, float y, float cb, float cr);
YCbCr getYCbCr(int xx, int yy);


void szary();

void setRGB555(int xx, int yy, Uint8 r, Uint8 g, Uint8 b);
void setRGB555(int xx, int yy, Uint16 rgb555);

SDL_Color getRGB555(int xx, int yy);
Uint16 getRGB555_(int xx, int yy);

SDL_Color getRGB555D(int xx, int yy);
Uint16 getRGB555D_(int xx, int yy);
void zaktualizujTabliceBayera4();

extern SDL_Color filtrPNG[320][200];

extern Uint8 filtrRGB555_lo[320][200];
extern Uint8 filtrRGB555_hi[320][200];


void filtrujPNG_Typ3();

void odfiltrujPNG_Typ3();

void filtrujYUV_Typ3(); void odfiltrujYUV_Typ3();

void filtrujRGB555_Typ3(); void odfiltrujRGB555_Typ3();

#endif // SM2025_PALETA_H_INCLUDED

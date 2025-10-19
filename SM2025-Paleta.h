// funkcje do redukcji kolorów i tworzenia palet
#ifndef SM2025_PALETA_H_INCLUDED
#define SM2025_PALETA_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>
#include <algorithm>
using namespace std;

// Struktura do zwracania wynikow
struct WynikStruct {
    SDL_Color paleta[64];      // Paleta kolorów
    Uint8 indeksy[64000];      //macierz zawierajaca indeksy z palety kolorów
};

struct YUV{;
    float y;
    float u;
    float v;
};
struct YIQ{
    float y;
    float i;
    float q;
};
struct YCbCr{
    float y;
    float cb;
    float cr;
};

struct Kolor{
    float r;
    float g;
    float b;
};

struct HSL{
    float h;
    float s;
    float l;
};

void setPixelByStruct(int x, int y, Kolor kolor);
float normalize(float part);
Kolor normalizeAll(float r,float g,float b);

void setYUV(int xx, int yy, float y, float u, float v);
YUV getYUV(int xx, int yy);

void setYIQ(int xx, int yy, float y, float i, float q);
YIQ getYIQ(int xx, int yy);

void setYCbCr(int xx, int yy, float y, float cb, float cr);
YCbCr getYCbCr(int xx, int yy);

void setHSL(int xx, int yy, float h, float s, float l);
HSL getHSL(int xx, int yy);


float test(float color , float var1, float var2);
Uint8 z24Kdo6K(SDL_Color kolor);
SDL_Color z6Kdo24K(Uint8 kolor6bit);
void paletaNarzucona6BIT(WynikStruct* wynik);
void narysujPalete6BIT(int px, int py, SDL_Color pal6[]);

void szary();
SDL_Color z6Kdo24Kszary(Uint8 szary6bit);
Uint8 z24Kdo6Kszary(SDL_Color kolor);
void paletaNarzuconaSzary6BIT(WynikStruct* wynik);


int dodajKolor6K(SDL_Color kolor);
int sprawdzKolor6K(SDL_Color kolor);
void czyscPalete6K();
void paletaWykryta6K(WynikStruct* wynik);
float normalizeHsl(float x);


#endif // SM2025_PALETA_H_INCLUDED

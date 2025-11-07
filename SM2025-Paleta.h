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

void subsample420_YUV(int width, int height);
void subsample420_YIQ(int width, int height);
void subsample420_YCbCr(int width, int height);
void subsample420_HSL(int width, int height);

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


void setRGB555(int xx, int yy, Uint8 r, Uint8 g, Uint8 b);
void setRGB555(int xx, int yy, Uint16 rgb555);
void setRGB565(int xx, int yy, Uint8 r, Uint8 g, Uint8 b);
void setRGB565(int xx, int yy, Uint16 rgb565);

SDL_Color getRGB555(int xx, int yy);
Uint16 getRGB555_(int xx, int yy);
SDL_Color getRGB565(int xx, int yy);
Uint16 getRGB565_(int xx, int yy);

SDL_Color getRGB555D(int xx, int yy);
Uint16 getRGB555D_(int xx, int yy);
SDL_Color getRGB565D(int xx, int yy);
Uint16 getRGB565D_(int xx, int yy);
void zaktualizujTabliceBayera4();

extern SDL_Color filtrPNG[320][200];
extern YUV filtrYUV[320][200];

extern Uint8 filtrRGB555_lo[320][200];
extern Uint8 filtrRGB555_hi[320][200];

extern YUV buforYUV[320][200];


extern Uint8 filtrRGB565_lo[320][200];
extern Uint8 filtrRGB565_hi[320][200];

void filtrujPNG_Typ1();
void filtrujPNG_Typ2();
void filtrujPNG_Typ3();
void filtrujPNG_Typ4();
void odfiltrujPNG_Typ1();
void odfiltrujPNG_Typ2();
void odfiltrujPNG_Typ3();
void odfiltrujPNG_Typ4();

void filtrujYUV_Typ1(); void odfiltrujYUV_Typ1();
void filtrujYUV_Typ2(); void odfiltrujYUV_Typ2();
void filtrujYUV_Typ3(); void odfiltrujYUV_Typ3();
void filtrujYUV_Typ4(); void odfiltrujYUV_Typ4();
void filtrujRGB555_Typ1(); void odfiltrujRGB555_Typ1();
void filtrujRGB555_Typ2(); void odfiltrujRGB555_Typ2();
void filtrujRGB555_Typ3(); void odfiltrujRGB555_Typ3();
void filtrujRGB555_Typ4(); void odfiltrujRGB555_Typ4();
void filtrujRGB565_Typ1(); void odfiltrujRGB565_Typ1();
void filtrujRGB565_Typ2(); void odfiltrujRGB565_Typ2();
void filtrujRGB565_Typ3(); void odfiltrujRGB565_Typ3();
void filtrujRGB565_Typ4(); void odfiltrujRGB565_Typ4();
void zapiszYUVDoBufora();

#endif // SM2025_PALETA_H_INCLUDED

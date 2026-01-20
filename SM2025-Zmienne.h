// zmienne globalne
#ifndef SM2025_ZMIENNE_H_INCLUDED
#define SM2025_ZMIENNE_H_INCLUDED

#include <SDL2/SDL.h>
#include "SM2025-Paleta.h"


#define szerokosc 640
#define wysokosc 400

#define hwidth 320
#define hheight 200

#define tytul "SM2025 - Projekt - Zespol 21"

extern SDL_Window* window;
extern SDL_Surface* screen;

extern int filtrTypPNG[200];
extern int filtrTypRGB555[200];
extern YCbCr filtrYCbCr[320][200];
extern YCbCr buforYCbCr[320][200];
extern SDL_Color buforPNG[320][200];
extern Uint16 buforRGB555[320][200];
extern Kolor subcolors[320][200];


extern float zaktualizowanaTablicaBayera4k[4][4];
extern float zaktualizowanaTablicaBayera4s[4][4];
extern int BLOK_DCT;

#endif // SM2025_ZMIENNE_H_INCLUDED

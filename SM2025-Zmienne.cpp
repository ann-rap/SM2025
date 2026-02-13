// zmienne globalne
#include "SM2025-Zmienne.h"
SDL_Window* window = NULL;
SDL_Surface* screen = NULL;

float zaktualizowanaTablicaBayera4k[4][4];
float zaktualizowanaTablicaBayera4s[4][4];


int filtrTypPNG[200];
int filtrTypRGB555[200];
YCbCr filtrYCbCr[320][200];
YCbCr buforYCbCr[320][200];
 int BLOK_DCT = 8;
 SDL_Color buforPNG[320][200];
 Uint16 buforRGB555[320][200];
 Kolor subcolors[320][200];

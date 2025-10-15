// zmienne globalne
#ifndef SM2025_ZMIENNE_H_INCLUDED
#define SM2025_ZMIENNE_H_INCLUDED

#include <SDL2/SDL.h>

#define szerokosc 640
#define wysokosc 400

#define tytul "SM2025 - Projekt - Zespol 21"


extern SDL_Window* window;
extern SDL_Surface* screen;


extern int ileKolorow;
extern SDL_Color paleta6[64];


extern SDL_Color paleta6k[64];
extern SDL_Color paleta6s[64];


extern int ileKubelkow;
extern Uint8 obrazekS[320*200];
extern SDL_Color obrazekK[320*200];
extern int obrazekIn[320*200];
extern int obrazekInS[320*200];
extern int indeksy[320][200];

#endif // SM2025_ZMIENNE_H_INCLUDED

// funkcje do redukcji kolorów i tworzenia palet
#ifndef SM2025_PALETA_H_INCLUDED
#define SM2025_PALETA_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>
using namespace std;

Uint8 z24Kdo6K(SDL_Color kolor);
SDL_Color z6Kdo24K(Uint8 kolor6bit);
void paletaNarzucona6BIT();
void narysujPalete6BIT(int px, int py, SDL_Color pal6[]);

void szary();
SDL_Color z6Kdo24Kszary(Uint8 szary6bit);
Uint8 z24Kdo6Kszary(SDL_Color kolor);
void paletaNarzuconaSzary6BIT();


int dodajKolor6K(SDL_Color kolor);
int sprawdzKolor6K(SDL_Color kolor);
void czyscPalete6K();
void paletaWykryta6K();


#endif // SM2025_PALETA_H_INCLUDED

// metody do algorytmu kwantyzacji (MedianCut)
#ifndef SM2025_MEDIANCUT_H_INCLUDED
#define SM2025_MEDIANCUT_H_INCLUDED

#include <SDL2/SDL.h>
#include "SM2025-Paleta.h"

void czyscPalete();


//Czarny-Bialy - 6 bit
int znajdzSasiadaBW6bit(Uint8 wartosc);
void MedianCutBW6bit(int start, int koniec, int iteracja);
void sortujKubelekBW6bit(int start, int koniec);
void sortujKubelekBWQuick6bit(int start, int koniec);
void czyscPalete6bit();
void narysujPaletaBW6bit(int px, int py, SDL_Color pal6b[]);
void paletaMedianCutBW6bit(WynikStruct* wynik);

//Kolorowe - 6 bit
void narysujPaleta6bit(int px, int py, SDL_Color pal6[]);
int znajdzSasiada6bit(SDL_Color kolor);
int partitionx6bit(int start, int koniec, Uint8 sortowanie);
void quickSort6bit(int start, int koniec, Uint8 sortowanie);
uint8_t najwiekszaRoznica6bit(int start, int koniec);
void MedianCut6bit(int start, int koniec, int iteracja);
void paletaMedianCut6bit(WynikStruct* wynik);


#endif // SM2025_MEDIANCUT_H_INCLUDED


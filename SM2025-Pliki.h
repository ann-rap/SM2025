// funkcje do operacji na plikach
#ifndef SM2025_PLIKI_H_INCLUDED
#define SM2025_PLIKI_H_INCLUDED

#include <stdio.h>
#include <math.h>
#include <fstream>
#include <vector>
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
ByteRunColors kompresjaObrazu(SDL_Color colors[], int len);
SDL_Color* dekompresjObrazu(ByteRunColors* colors);
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

/*
Funkcje i struktury LZW
*/
struct LZW {
    Uint16* tab;  // zakodowane kody LZW
    int     len;  // ile elementów w tablicy

    LZW(Uint16* t = nullptr, int l = 0)
        : tab(t), len(l) {}
};

struct LZWColors {
    LZW* rtab;
    LZW* gtab;
    LZW* btab;
    LZWColors() : rtab(nullptr), gtab(nullptr), btab(nullptr) {}
    LZWColors(LZW* r, LZW* g, LZW* b) : rtab(r), gtab(g), btab(b) {}
};

struct slowo {
    Uint16 kod = 0;
    Uint8  dlugosc = 0;
    Uint8  element[4096];
    bool   wSlowniku = false;
};

LZW* LZWKompresja(int wejscie[], int dlugosc);
LZWColors kompresjaObrazu_LZW(Kolor colors[], int len);
LZW* LZWDekompresja(const LZW* skompresowane);
Kolor* dekompresjaObrazu_LZW(LZWColors* colors);
void LZWinicjalizacja();

int dodajDoSlownika(slowo nowy, bool czyWyswietlac = true);
slowo noweSlowo();
slowo noweSlowo(Uint8 znak);
slowo polaczSlowo(slowo aktualneSlowo, Uint8 znak);
void wyswietlSlowo(slowo aktualneSlowo);
int znajdzWSlowniku(slowo szukany);
bool porownajSlowa(slowo slowo1, slowo slowo2);
void wyswietlSlownik();

//void zapisz_LZW(LZW* lzw, const char* filename);
//void pokazStatystykiKompresji_LZW(int rozmiarWejsciowy, LZW* wynik);
//LZW* wczytaj_LZW(const char* filename);

void zapiszPojedynczyLZW(std::ofstream& out, LZW* lzw);
void zapisz_LZW(LZWColors* colors, const char* filename);
void wczytajPojedynczyLZW(std::ifstream& in, LZW* lzw);
void wczytaj_LZW(LZWColors* colors, const char* filename);




#endif // SM2025_PLIKI_H_INCLUDED

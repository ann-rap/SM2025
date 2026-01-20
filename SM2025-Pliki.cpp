// funkcje do operacji na plikach
#include "SM2025-Funkcje.h"
#include "SM2025-Zmienne.h"
#include "SM2025-Paleta.h"
#include "SM2025-MedianCut.h"
#include "SM2025-Pliki.h"
#include <fstream>

RLE* RLEKompresja(int wejscie[], int dlugosc) {
     int i = 0;
    int16_t* result_tab = new int16_t[dlugosc * 2]; // Maksymalny rozmiar
    int ri = 0;

    while (i < dlugosc) {
        // Sekwencja powtarzającej się
        if ((i < dlugosc - 1) && (wejscie[i] == wejscie[i + 1])) {
            // Mierzymy długość sekwencji
            int j = 1;
            while ((i + j < dlugosc) &&
                   (wejscie[i] == wejscie[i + j]) &&
                   (j < 254)) { // Max 254 powtórzeń
                j++;
            }
            result_tab[ri++] = (int16_t)j; // Liczba powtórzeń
            result_tab[ri++] = (int16_t)wejscie[i];

            i += j;
        }
        // Sekwencja różnych bajtów
        else {
            int j = 0;
            while ((i + j < dlugosc) && (j < 127)) { //max 127 roznych
                // Jeśli następne 2 są takie same, to kończymy sekwencję różnych
                if ((i + j < dlugosc - 1) && (wejscie[i + j] == wejscie[i + j + 1])) {
                    break;
                }
                j++;
            }

            if (j == 0) {
                j = 1;
            }

            result_tab[ri++] = 0;
            result_tab[ri++] = (int16_t)j;

            for (int k = 0; k < j; k++) {
                result_tab[ri++] = (int16_t)wejscie[i + k];
            }

            // Dodajemy 0 na koncu jesli nieparzysta liczba
            if (j % 2 == 1) {
                result_tab[ri++] = 0;
            }

            i += j;
        }
    }

    return new RLE(result_tab, ri);
}


RLE* RLEDekompresja(int16_t wejscie[], int dlugosc) {
    int16_t* result_tab = new int16_t[hwidth * hheight];
    int i = 0;
    int tab_index = 0;

    while (i < dlugosc) {
        int16_t first = wejscie[i++];

        if (first == 0) {
            // Sekwencja różnych bajtów
            if (i >= dlugosc) break;

            int16_t count = wejscie[i++];

            // Kopiujemy 'count' różnych bajtów
            for (int k = 0; k < count && i < dlugosc; k++) {
                result_tab[tab_index++] = wejscie[i++];
            }
            if (count % 2 == 1 && i < dlugosc) {
                i++;
    }
        }
        else {
            // Sekwencja powtarzająca się
            if (i >= dlugosc) break;

            int repeat_count = first;
            int16_t value = wejscie[i++];

            for (int k = 0; k < repeat_count; k++) {
                result_tab[tab_index++] = value;
            }
        }
    }

    return new RLE(result_tab, tab_index);
}

RLEColors kompresjaObrazu_RLE(Kolor colors[], int len) {
    int* rt = new int[len];
    int* gt = new int[len];
    int* bt = new int[len];

    for (int i = 0; i < len; i++) {
        rt[i] = colors[i].c1;
        gt[i] = colors[i].c2;
        bt[i] = colors[i].c3;
    }

    RLE* r_rle = RLEKompresja(rt, len);
    RLE* g_rle = RLEKompresja(gt, len);
    RLE* b_rle = RLEKompresja(bt, len);

    delete[] rt;
    delete[] gt;
    delete[] bt;

    return RLEColors(r_rle, g_rle, b_rle);
}

Kolor* dekompresjaObrazu_RLE(RLEColors* colors) {
    Kolor* result = new Kolor[hwidth * hheight];

    RLE* r_rle = RLEDekompresja(colors->rtab->tab, colors->rtab->len);
    RLE* g_rle = RLEDekompresja(colors->gtab->tab, colors->gtab->len);
    RLE* b_rle = RLEDekompresja(colors->btab->tab, colors->btab->len);

    if (r_rle->len == g_rle->len && r_rle->len == b_rle->len) {
        for (int i = 0; i < r_rle->len; i++) {
            result[i].c1 = static_cast<Uint8>(r_rle->tab[i]);
            result[i].c2 = static_cast<Uint8>(g_rle->tab[i]);
            result[i].c3 = static_cast<Uint8>(b_rle->tab[i]);
        }
    }

    delete r_rle;
    delete g_rle;
    delete b_rle;

    return result;
}

void zapiszPojedynczyRLE(std::ofstream& out, RLE* rle) {
    if (!rle) {
        int32_t zero = 0;
        out.write(reinterpret_cast<char*>(&zero), sizeof(zero));
        return;
    }
    int32_t len = rle->len;
    out.write(reinterpret_cast<char*>(&len), sizeof(len));
    out.write(reinterpret_cast<char*>(rle->tab), len * sizeof(int16_t));
}

void zapisz_RLE(RLEColors* colors,char const* filename) {
    std::ofstream out(filename, std::ios::binary | std::ios::trunc);
    if (out.good()) {
        zapiszPojedynczyRLE(out, colors->rtab);
        zapiszPojedynczyRLE(out, colors->gtab);
        zapiszPojedynczyRLE(out, colors->btab);
        out.close();
    } else {
        std::cerr << "Nie udało się otworzyć pliku do zapisu RLE!" << std::endl;
    }
}

void wczytajPojedynczyRLE(std::ifstream& in, RLE* rle) {
    int32_t len = 0;
    in.read(reinterpret_cast<char*>(&len), sizeof(len));
    rle->len = len;
    if (len > 0) {
        rle->tab = new int16_t[len];
        in.read(reinterpret_cast<char*>(rle->tab), len * sizeof(int16_t));
    } else {
        rle->tab = nullptr;
    }
}

void wczytaj_RLE(RLEColors* colors, const char* filename) {
    std::ifstream in(filename, std::ios::binary);
    if (in.good()) {
        wczytajPojedynczyRLE(in, colors->rtab);
        wczytajPojedynczyRLE(in, colors->gtab);
        wczytajPojedynczyRLE(in, colors->btab);
        in.close();
        std::cout << "RLE wczytane pomyślnie z: " << filename << std::endl;
    } else {
        std::cerr << "Nie udało się otworzyć pliku do odczytu RLE!" << std::endl;
    }
}


void zapiszRLEdoStrumienia(ofstream& plik, RLEColors* rle) {
    // Zakładamy, że masz funkcję zapiszPojedynczyRLE przyjmującą ofstream&
    // Jeśli Twoja funkcja przyjmuje tylko nazwę pliku, musisz zmienić ją na wersję przyjmującą referencję do strumienia
    zapiszPojedynczyRLE(plik, rle->rtab);
    zapiszPojedynczyRLE(plik, rle->gtab);
    zapiszPojedynczyRLE(plik, rle->btab);
}








// funkcje do operacji na plikach
#include "SM2025-Funkcje.h"
#include "SM2025-Zmienne.h"
#include "SM2025-Paleta.h"
#include "SM2025-MedianCut.h"
#include "SM2025-Pliki.h"
#include <fstream>

ByteRun* ByteRunKompresja(float wejscie[], int dlugosc) {
    int i = 0;
    int16_t* result_tab = new int16_t[dlugosc];
    int ri=0;

    // dopóki wszystkie dane nie zosta³y przetworzone (skompresowane)
    while (i < dlugosc) {

        // sekwencja powtarzaj¹cych siê przynajmniej dwóch bajtów
        if ((i < dlugosc - 1) && (wejscie[i] == wejscie[i + 1])) {

            // mierzymy d³ugoœæ sekwencji
            int j = 0;
            while ((i + j < dlugosc - 1) &&
                   (wejscie[i + j] == wejscie[i + 1 + j]) &&
                   (j < 127)) {
                j++;
            }

            // wypisujemy spakowan¹ sekwencjê
            result_tab[ri++] =-j;
            result_tab[ri++] = (int16_t)wejscie[i + j];

            i += (j + 1);

        // sekwencja ró¿nych bajtów
        } else {
            int j = 0;

            while ((i + j < dlugosc - 1) &&
                   (wejscie[i + j] != wejscie[i + j + 1]) &&
                   (j < 128)) {
                j++;
            }

            // dodajemy jeszcze koñcówkê
            if ((i + j == dlugosc - 1) && (j < 128)) {
                j++;
            }

            // wypisujemy spakowan¹ sekwencjê
            // cout<<"("<<(j-1)<<"), ";
            for (int k = 0; k < j; k++) {
                result_tab[ri++] =(int16_t)wejscie[i + k];
            }

            i += j;
        }
    }
    return new ByteRun(result_tab,ri);
}

// Funkcja dekompresji pojedynczego kanału ByteRun
ByteRun* ByteRunDekompresja(int16_t wejscie[], int dlugosc) {
    int16_t* result_tab = new int16_t[hwidth * hheight];
    int i = 0;
    int tab_index = 0;

    while (i < dlugosc) {
        if (wejscie[i] < 0) {
            int j = -wejscie[i];
            int16_t value = wejscie[i + 1];

            for (int k = 0; k <= j; k++) {
                result_tab[tab_index++] = value;
            }
            i += 2;
        } else {
            result_tab[tab_index++] = wejscie[i++];
        }
    }

    return new ByteRun(result_tab, tab_index);
}



ByteRunColors kompresjaObrazu(Kolor colors[], int len){
    float* rt = new float[len];
    float* gt = new float[len];
    float* bt = new float[len];
    for(int i=0; i<len;i++){
        rt[i] = colors[i].c1;
        gt[i] = colors[i].c2;
        bt[i] = colors[i].c3;
    }
    ByteRun* r_br = ByteRunKompresja(rt,len);
    ByteRun* g_br = ByteRunKompresja(gt,len);
    ByteRun* b_br = ByteRunKompresja(bt,len);

    return ByteRunColors(r_br,g_br,b_br);
}

Kolor* dekompresjObrazu(ByteRunColors* colors){
    Kolor* result= new Kolor[hwidth*hheight];
    ByteRun* r_br = ByteRunDekompresja(colors->rtab->tab, colors->rtab->len);
    ByteRun* g_br =  ByteRunDekompresja(colors->gtab->tab, colors->gtab->len);
    ByteRun* b_br =  ByteRunDekompresja(colors->btab->tab, colors->btab->len);

    if(r_br->len == g_br->len && r_br->len == b_br->len){
        for(int i= 0; i<r_br->len;i++){
            result[i].c1 = static_cast<Uint8>(r_br->tab[i]);
            result[i].c2 = static_cast<Uint8>(g_br->tab[i]);
            result[i].c3= static_cast<Uint8>(b_br->tab[i]);

        }
    }
    return result;

}

void zapiszPojedynczyByteRun(std::ofstream& out, ByteRun* br) {
    if (!br) {
        int32_t zero = 0;
        out.write(reinterpret_cast<char*>(&zero), sizeof(zero));
        return;
    }
    int32_t len = br->len;
    out.write(reinterpret_cast<char*>(&len), sizeof(len));
    out.write(reinterpret_cast<char*>(br->tab), len * sizeof(int16_t));
}

void zapisz(ByteRunColors* colors) {
    std::ofstream out("obrazek.z21", std::ios::binary | std::ios::trunc);
    if (out.good()) {
        zapiszPojedynczyByteRun(out, colors->rtab);
        zapiszPojedynczyByteRun(out, colors->gtab);
        zapiszPojedynczyByteRun(out, colors->btab);
        out.close();
        std::cout << "Zapisano pomyślnie." << std::endl;
    } else {
        std::cerr << "Nie udało się otworzyć pliku do zapisu!" << std::endl;
    }
}

void wczytajPojedynczyByteRun(std::ifstream& in, ByteRun* br) {
    int32_t len = 0;
    in.read(reinterpret_cast<char*>(&len), sizeof(len));
    br->len = len;
    if (len > 0) {
        br->tab = new int16_t[len];
        in.read(reinterpret_cast<char*>(br->tab), len * sizeof(int16_t));
    } else {
        br->tab = nullptr;
    }
}

void wczytaj(ByteRunColors* colors) {
    std::ifstream in("obrazek.z21", std::ios::binary);
    if (in.good()) {
        wczytajPojedynczyByteRun(in, colors->rtab);
        wczytajPojedynczyByteRun(in, colors->gtab);
        wczytajPojedynczyByteRun(in, colors->btab);
        in.close();
        std::cout << "Wczytano pomyślnie." << std::endl;
    } else {
        std::cerr << "Nie udało się otworzyć pliku do odczytu!" << std::endl;
    }
}


////////////////////////////////////
///////RLE kompresja ///////////////
////////////////////////////////////

/*
*@param - tablica wejscie[] zawierajaca wartosci 0-255 oznaczajace kolory RGB
*@param - dlugosc - ile znajduje sie elementow w tablicy
*
* @return RLE - strukture z zapisanym wynikiem kompresji RLE
*/
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

void zapisz_RLE(RLEColors* colors, const char* filename) {
    std::ofstream out(filename, std::ios::binary | std::ios::trunc);
    if (out.good()) {
        zapiszPojedynczyRLE(out, colors->rtab);
        zapiszPojedynczyRLE(out, colors->gtab);
        zapiszPojedynczyRLE(out, colors->btab);
        out.close();
        std::cout << "RLE zapisane pomyślnie do: " << filename << std::endl;
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

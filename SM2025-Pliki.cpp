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



ByteRunColors kompresjaObrazu(SDL_Color colors[], int len){
    float* rt = new float[len];
    float* gt = new float[len];
    float* bt = new float[len];
    for(int i=0; i<len;i++){
        rt[i] = colors[i].r;
        gt[i] = colors[i].g;
        bt[i] = colors[i].b;
    }
    ByteRun* r_br = ByteRunKompresja(rt,len);
    ByteRun* g_br = ByteRunKompresja(gt,len);
    ByteRun* b_br = ByteRunKompresja(bt,len);

    return ByteRunColors(r_br,g_br,b_br);
}


SDL_Color* dekompresjObrazu(ByteRunColors* colors){
    SDL_Color* result= new SDL_Color[hwidth*hheight];
    ByteRun* r_br = ByteRunDekompresja(colors->rtab->tab, colors->rtab->len);
    ByteRun* g_br =  ByteRunDekompresja(colors->gtab->tab, colors->gtab->len);
    ByteRun* b_br =  ByteRunDekompresja(colors->btab->tab, colors->btab->len);

    if(r_br->len == g_br->len && r_br->len == b_br->len){
        for(int i= 0; i<r_br->len;i++){
            result[i].r = static_cast<Uint8>(r_br->tab[i]);
            result[i].g = static_cast<Uint8>(g_br->tab[i]);
            result[i].b = static_cast<Uint8>(b_br->tab[i]);

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

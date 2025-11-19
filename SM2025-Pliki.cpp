// funkcje do operacji na plikach
#include "SM2025-Funkcje.h"
#include "SM2025-Zmienne.h"
#include "SM2025-Paleta.h"
#include "SM2025-MedianCut.h"
#include "SM2025-Pliki.h"
#include <fstream>

ByteRun* ByteRunKompresja(int wejscie[], int dlugosc) {
    int i = 0;
    int* result_tab = new int[dlugosc];
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
            result_tab[ri++] = (int)wejscie[i + j];

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
                result_tab[ri++] =(int)wejscie[i + k];
            }

            i += j;
        }
    }
    return new ByteRun(result_tab,ri);
}

ByteRun* ByteRunDekompresja(int wejscie[], int dlugosc) {
    int* result_tab= new int[hwidth*hheight];
    int i = 0;
    int tab_index=0;

    while (i < dlugosc) {

        if (wejscie[i] < 0) {

            int j = -wejscie[i];
            int value = wejscie[i + 1];


            for (int k = 0; k <= j; k++) {
                result_tab[tab_index] = value;
                tab_index++;
            }

            i += 2;
        }

        else {
            result_tab[tab_index++] = wejscie[i++];
        }
    }
    return new ByteRun(result_tab,tab_index);
}


ByteRunColors kompresjaObrazu(SDL_Color colors[], int len){
    int* rt = new int[len];
    int* gt = new int[len];
    int* bt = new int[len];
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
            result[i].r = r_br->tab[i];
            result[i].g = g_br->tab[i];
            result[i].b = b_br->tab[i];
        }
    }
    return result;

}

void zapiszPojedynczyByteRun(fstream& out, ByteRun* br) {
    if (br == nullptr) {
        out << 0 << endl;
        return;
    }
    out << br->len << endl;

    for (int i = 0; i < br->len; i++) {
        out << br->tab[i] << " ";
    }
    out << endl;
}
void zapisz(ByteRunColors* colors) {
    fstream out;
    out.open("obrazek.z21", ios::out | ios::trunc);

    if (out.good()) {
        zapiszPojedynczyByteRun(out, colors->rtab);
        zapiszPojedynczyByteRun(out, colors->gtab);
        zapiszPojedynczyByteRun(out, colors->btab);
        out.close();
        cout << "Zapisano pomyslnie." << endl;
    } else {
        cerr << "Nie udalo sie otworzyc pliku do zapisu!" << endl;
    }
}

void wczytajPojedynczyByteRun(fstream& in, ByteRun* br) {
    int dlugosc;
    in >> dlugosc;

    br->len = dlugosc;
    if (dlugosc > 0) {
        br->tab = new int[dlugosc];
        for (int i = 0; i < dlugosc; i++) {
            in >> br->tab[i];
        }
    } else {
        br->tab = nullptr;
    }
}
void wczytaj(ByteRunColors* colors) {
    fstream in;
    in.open("obrazek.z21", ios::in);

    if (in.good()) {
        wczytajPojedynczyByteRun(in, colors->rtab);
        wczytajPojedynczyByteRun(in, colors->gtab);
        wczytajPojedynczyByteRun(in, colors->btab);

        in.close();
        cout << "Wczytano pomyslnie." << endl;
    } else {
        cerr << "Nie udalo sie otworzyc pliku do odczytu!" << endl;
    }
}


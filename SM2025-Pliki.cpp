// funkcje do operacji na plikach
#include "SM2025-Funkcje.h"
#include "SM2025-Zmienne.h"
#include "SM2025-Paleta.h"
#include "SM2025-MedianCut.h"
#include "SM2025-Pliki.h"

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

        // przypadek RUN-LENGTH (nag³ówek ujemny)
        if (wejscie[i] < 0) {

            int j = -wejscie[i];   // j = ile powtórzeñ minus jeden
            int value = wejscie[i + 1]; // bajt powtarzany

            // wypisujemy (j + 1) kopii value
            for (int k = 0; k <= j; k++) {
                result_tab[tab_index] = value;
                tab_index++;
            }

            i += 2;
        }

        // przypadek RAW — pojedyncze niepowtarzaj¹ce siê bajty
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
     cout<<r_br->len<<" "
        <<g_br->len<<" "
        <<b_br->len<<endl;
    return ByteRunColors(r_br,g_br,b_br);
}


SDL_Color* dekompresjObrazu(ByteRunColors* colors){
    SDL_Color* result= new SDL_Color[hwidth*hheight];
    ByteRun* r_br = ByteRunDekompresja(colors->rtab->tab, colors->rtab->len);
    ByteRun* g_br =  ByteRunDekompresja(colors->gtab->tab, colors->gtab->len);
    ByteRun* b_br =  ByteRunDekompresja(colors->btab->tab, colors->btab->len);
    cout<<r_br->len<<" "
        <<g_br->len<<" "
        <<b_br->len<<endl;
    if(r_br->len == g_br->len && r_br->len == b_br->len){
        for(int i= 0; i<r_br->len;i++){
            result[i].r = r_br->tab[i];
            result[i].g = g_br->tab[i];
            result[i].b = b_br->tab[i];
        }
    }
    return result;

}

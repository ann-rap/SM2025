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

//LZW

int rozmiarSlownika = 0;
slowo slownik[65535];
std::vector<int> wynikKompresji;

LZW* LZWKompresja(int wejscie[], int dlugosc) {

    LZWinicjalizacja();

    // z zapasem – dla laboratoriów w zupełności wystarczy
    Uint16* result_tab = new Uint16[dlugosc * 2];
    int ri = 0;  // indeks w tablicy wynikowej

    slowo aktualneSlowo = noweSlowo();
    slowo slowoZnak;
    Uint8 znak;
    int   kod;

    int i = 0;

    while (i < dlugosc) {

        // pobieramy znak z wejścia
        znak = (Uint8)wejscie[i];

        // łączymy aktualne słowo ze znakiem
        slowoZnak = polaczSlowo(aktualneSlowo, znak);

        // sprawdzamy, czy slowo+znak jest w słowniku
        kod = znajdzWSlowniku(slowoZnak);

        if (kod < 0) {
            // NIE ma w słowniku:
            // 1) wypisujemy (zapisujemy) kod aktualnego słowa
            //    (zakładamy, że aktualneSlowo.kod jest poprawnie ustawione)
            result_tab[ri++] = (Uint16)aktualneSlowo.kod;

            // 2) dodajemy slowo+znak do słownika
            dodajDoSlownika(slowoZnak, false);

            // 3) nowe aktualne słowo = sam znak
            aktualneSlowo      = noweSlowo(znak);
            aktualneSlowo.kod  = znajdzWSlowniku(aktualneSlowo);
            aktualneSlowo.wSlowniku = true;
        }
        else {
            // TAK – slowo+znak jest już w słowniku
            aktualneSlowo      = slowoZnak;
            aktualneSlowo.kod  = kod;
            aktualneSlowo.wSlowniku = true;
        }

        i++;
    }

    // po pętli wypisujemy jeszcze ostatnie słowo
    result_tab[ri++] = (Uint16)aktualneSlowo.kod;

    // Zwracamy wynik w strukturze LZW (tablica + długość)
    return new LZW(result_tab, ri);
}

LZWColors kompresjaObrazu_LZW(Kolor colors[], int len) {
    int* rt = new int[len];
    int* gt = new int[len];
    int* bt = new int[len];

    for (int i = 0; i < len; i++) {
        rt[i] = colors[i].c1;
        gt[i] = colors[i].c2;
        bt[i] = colors[i].c3;
    }

    LZW* r_lzw = LZWKompresja(rt, len);
    LZW* g_lzw = LZWKompresja(gt, len);
    LZW* b_lzw = LZWKompresja(bt, len);

    delete[] rt;
    delete[] gt;
    delete[] bt;

    return LZWColors(r_lzw, g_lzw, b_lzw);
}

LZW* LZWDekompresja(Uint16* kody, int n) {

    LZWinicjalizacja();   // słownik startowy (0..255 albo 0..3 – zależnie jak ustawiłaś)

    if (!kody || n <= 0) {
        return new LZW(); // pusty wynik
    }

    // wynikowa tablica symboli (z zapasem)
    Uint16* wynik_tab = new Uint16[n * 4096];
    int wi = 0;

    // pierwszy kod
    int staryKod = kody[0];
    if (staryKod < 0 || staryKod >= rozmiarSlownika || !slownik[staryKod].wSlowniku) {
        std::cerr << "Blad dekompresji: pierwszy kod poza zakresem!\n";
        delete[] wynik_tab;
        return new LZW();
    }

    slowo stareSlowo = slownik[staryKod];

    // zapisz pierwsze słowo do wyniku
    for (int j = 0; j < stareSlowo.dlugosc; j++) {
        wynik_tab[wi++] = stareSlowo.element[j];
    }

    // kolejne kody
    for (int i = 1; i < n; i++) {

        int nowyKod = kody[i];
        slowo aktualneSlowo;

        if (nowyKod >= 0 && nowyKod < rozmiarSlownika && slownik[nowyKod].wSlowniku) {
            // normalny przypadek – kod jest w słowniku
            aktualneSlowo = slownik[nowyKod];
        }
        else {
            // specjalny przypadek LZW: słowo = stareSlowo + pierwszy znak stareSlowo
            aktualneSlowo = noweSlowo();
            aktualneSlowo.dlugosc = stareSlowo.dlugosc + 1;

            for (int j = 0; j < stareSlowo.dlugosc; j++) {
                aktualneSlowo.element[j] = stareSlowo.element[j];
            }
            aktualneSlowo.element[stareSlowo.dlugosc] = stareSlowo.element[0];
        }

        // zapisujemy aktualne słowo do wyniku
        for (int j = 0; j < aktualneSlowo.dlugosc; j++) {
            wynik_tab[wi++] = aktualneSlowo.element[j];
        }

        // dodajemy do słownika: stareSlowo + pierwszy znak aktualneSlowo
        slowo nowe = noweSlowo();
        nowe.dlugosc = stareSlowo.dlugosc + 1;

        for (int j = 0; j < stareSlowo.dlugosc; j++) {
            nowe.element[j] = stareSlowo.element[j];
        }
        nowe.element[stareSlowo.dlugosc] = aktualneSlowo.element[0];

        dodajDoSlownika(nowe, false);

        // przesuwamy
        stareSlowo = aktualneSlowo;
        staryKod   = nowyKod;
    }

    // wynik zwracamy w strukturze LZW
    return new LZW(wynik_tab, wi);
}


Kolor* dekompresjaObrazu_LZW(LZWColors* colors) {
    Kolor* result = new Kolor[hwidth * hheight];

    LZW* r_lzw = LZWDekompresja(colors->rtab->tab, colors->rtab->len);
    LZW* g_lzw = LZWDekompresja(colors->gtab->tab, colors->gtab->len);
    LZW* b_lzw = LZWDekompresja(colors->btab->tab, colors->btab->len);

    if (r_lzw->len == g_lzw->len && r_lzw->len == b_lzw->len) {
        for (int i = 0; i < r_lzw->len; i++) {
            result[i].c1 = static_cast<Uint8>(r_lzw->tab[i]);
            result[i].c2 = static_cast<Uint8>(g_lzw->tab[i]);
            result[i].c3 = static_cast<Uint8>(b_lzw->tab[i]);
        }
    }

    delete[] r_lzw->tab; delete r_lzw;
    delete[] g_lzw->tab; delete g_lzw;
    delete[] b_lzw->tab; delete b_lzw;

    return result;
}



void LZWinicjalizacja() {

    rozmiarSlownika = 0;

    // Czyścimy cały słownik
    for (int s = 0; s < 65535; s++) {
        slownik[s].kod = 0;
        slownik[s].dlugosc = 0;
        slownik[s].wSlowniku = false;
        memset(slownik[s].element, 0, sizeof(slownik[s].element));
    }

    // Dodajemy słowa jednoelementowe (0–255)
    slowo noweSlowo;
    for (int s = 0; s < 255; s++) {
        noweSlowo.dlugosc = 1;
        noweSlowo.element[0] = s;
        noweSlowo.kod = dodajDoSlownika(noweSlowo);
    }
}

int dodajDoSlownika(slowo nowy, bool czyWyswietlac){

    // sprawdzamy, czy nie przekraczamy rozmiaru słownika
    if (rozmiarSlownika < 4096) {

        Uint16 nr = rozmiarSlownika;
        slownik[nr].kod = nr;
        slownik[nr].dlugosc = nowy.dlugosc;

        // kopiujemy tablicę element z nowego elementu do słownika
        copy(begin(nowy.element), end(nowy.element),
             begin(slownik[nr].element));

        slownik[nr].wSlowniku = true;

        /*if (czyWyswietlac)
            wyswietlSlowo(slownik[nr]);*/

        rozmiarSlownika++;
        return nr;
    }

    return -1;
}

slowo noweSlowo() {
    slowo noweSlowo;
    noweSlowo.kod = 0;
    noweSlowo.dlugosc = 0;
    noweSlowo.wSlowniku = false;
    return noweSlowo;
}

slowo noweSlowo(Uint8 znak) {
    slowo noweSlowo;
    noweSlowo.kod = 0;
    noweSlowo.dlugosc = 1;
    noweSlowo.element[0] = znak;
    noweSlowo.wSlowniku = false;
    return noweSlowo;
}

slowo polaczSlowo(slowo aktualneSlowo, Uint8 znak) {

    slowo noweSlowo;

    // sprawdzamy, czy słowo nie zawiera więcej niż 4096 elementów
    // warto tu rozważyć inny sposób tworzenia listy znaków, np. vector
    if (aktualneSlowo.dlugosc < 4096) {

        noweSlowo.kod = 0;
        noweSlowo.dlugosc = aktualneSlowo.dlugosc + 1;
        noweSlowo.wSlowniku = false;

        // kopiujemy dotychczasowe elementy słowa
        copy(begin(aktualneSlowo.element),
             end(aktualneSlowo.element),
             begin(noweSlowo.element));

        // dopisujemy nowy znak na końcu
        noweSlowo.element[aktualneSlowo.dlugosc] = znak;

        return noweSlowo;
    }
    else {
        // awaryjnie zwracamy puste słowo
        cout << "UWAGA! Przepelnienie rozmiaru znakow w pojedynczym slowie!" << endl;

        noweSlowo.kod = 0;
        noweSlowo.dlugosc = 0;
        noweSlowo.wSlowniku = false;
        noweSlowo.element[0] = znak;

        return noweSlowo;
    }
}

void wyswietlSlowo(slowo aktualneSlowo) {

    if (aktualneSlowo.wSlowniku)
        cout << "[" << aktualneSlowo.kod << "] ";
    else
        cout << "[X] ";

    for (int s = 0; s < aktualneSlowo.dlugosc; s++) {
        cout << (int)aktualneSlowo.element[s];
        if (s < aktualneSlowo.dlugosc - 1)
            cout << ", ";
    }

    cout << endl;
}

int znajdzWSlowniku(slowo szukany) {

    for (int nr = 0; nr < rozmiarSlownika; nr++) {
        if (porownajSlowa(slownik[nr], szukany))
            return nr;
    }

    return -1;
}

bool porownajSlowa(slowo slowo1, slowo slowo2) {

    if (slowo1.dlugosc != slowo2.dlugosc)
        return false;

    for (int s = 0; s < slowo1.dlugosc; s++) {
        if (slowo1.element[s] != slowo2.element[s])
            return false;
    }

    return true;
}

void wyswietlSlownik() {
    for (int nr = 0; nr < rozmiarSlownika; nr++) {
        wyswietlSlowo(slownik[nr]);
    }
}

//Liczby
/*
void zapisz_LZW(LZW* lzw, const char* filename) {
    std::ofstream out(filename, std::ios::binary | std::ios::trunc);

    if (!out.good()) {
        std::cerr << "Nie udalo sie otworzyc pliku do zapisu LZW!" << std::endl;
        return;
    }

    int32_t len = (lzw ? lzw->len : 0);
    out.write(reinterpret_cast<char*>(&len), sizeof(len));

    if (len > 0 && lzw->tab != nullptr) {
        out.write(reinterpret_cast<char*>(lzw->tab),
                  len * sizeof(Uint16));
    }

    out.close();
    std::cout << "LZW zapisane pomyslnie do: " << filename << std::endl;
}

LZW* wczytaj_LZW(const char* filename) {
    std::ifstream in(filename, std::ios::binary);

    if (!in.good()) {
        std::cerr << "Nie udalo sie otworzyc pliku do odczytu LZW!" << std::endl;
        return nullptr;
    }

    int32_t len = 0;
    in.read(reinterpret_cast<char*>(&len), sizeof(len));

    if (len <= 0) {
        in.close();
        return new LZW(nullptr, 0);
    }

    Uint16* tab = new Uint16[len];
    in.read(reinterpret_cast<char*>(tab), len * sizeof(Uint16));

    in.close();
    std::cout << "LZW wczytane pomyslnie z: " << filename << std::endl;

    return new LZW(tab, len);
}


void pokazStatystykiKompresji_LZW(int rozmiarWejsciowy, LZW* wynik) {

    if (!wynik || wynik->len == 0) {
        std::cout << "Brak danych do statystyk LZW." << std::endl;
        return;
    }

    int rozmiarKompresji = wynik->len;

    double stopien  = (double)rozmiarWejsciowy / (double)rozmiarKompresji;
    double procent  = (1.0 - (double)rozmiarKompresji / (double)rozmiarWejsciowy) * 100.0;

    std::cout << "=== STATYSTYKI KOMpresji LZW ===" << std::endl;
    std::cout << "Rozmiar oryginalny:  " << rozmiarWejsciowy << std::endl;
    std::cout << "Rozmiar po kompresji:" << rozmiarKompresji << std::endl;
    std::cout << "Stopien kompresji:   " << stopien << std::endl;
    std::cout << "Zmniejszenie danych: " << procent << "%" << std::endl;
}

*/

//Obraz

void zapiszPojedynczyLZW(std::ofstream& out, LZW* lzw) {
    // jeśli brak danych – zapisujemy długość = 0
    if (!lzw || lzw->len <= 0 || lzw->tab == nullptr) {
        int32_t zero = 0;
        out.write(reinterpret_cast<char*>(&zero), sizeof(zero));
        return;
    }

    int32_t len = lzw->len;
    out.write(reinterpret_cast<char*>(&len), sizeof(len));
    out.write(reinterpret_cast<char*>(lzw->tab), len * sizeof(Uint16));
}

void zapisz_LZW(LZWColors* colors, const char* filename) {
    std::ofstream out(filename, std::ios::binary | std::ios::trunc);
    if (out.good()) {
        zapiszPojedynczyLZW(out, colors->rtab);
        zapiszPojedynczyLZW(out, colors->gtab);
        zapiszPojedynczyLZW(out, colors->btab);
        out.close();
        std::cout << "LZW zapisane pomyslnie do: " << filename << std::endl;
    } else {
        std::cerr << "Nie udało się otworzyć pliku do zapisu LZW!" << std::endl;
    }
}

void wczytajPojedynczyLZW(std::ifstream& in, LZW* lzw) {
    int32_t len = 0;
    in.read(reinterpret_cast<char*>(&len), sizeof(len));
    lzw->len = len;

    if (lzw->tab) {
        delete[] lzw->tab;
        lzw->tab = nullptr;
    }

    if (len > 0) {
        lzw->tab = new Uint16[len];
        in.read(reinterpret_cast<char*>(lzw->tab), len * sizeof(Uint16));
    } else {
        lzw->tab = nullptr;
    }
}

void wczytaj_LZW(LZWColors* colors, const char* filename) {
    std::ifstream in(filename, std::ios::binary);
    if (in.good()) {
        if (!colors->rtab) colors->rtab = new LZW();
        if (!colors->gtab) colors->gtab = new LZW();
        if (!colors->btab) colors->btab = new LZW();

        wczytajPojedynczyLZW(in, colors->rtab);
        wczytajPojedynczyLZW(in, colors->gtab);
        wczytajPojedynczyLZW(in, colors->btab);

        in.close();
        std::cout << "LZW wczytane pomyslnie z: " << filename << std::endl;
    } else {
        std::cerr << "Nie udało się otworzyć pliku do odczytu LZW!" << std::endl;
    }
}












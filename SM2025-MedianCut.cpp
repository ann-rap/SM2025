// metody do algorytmu kwantyzacji (MedianCut)
#include "SM2025-MedianCut.h"
#include "SM2025-Zmienne.h"
#include "SM2025-Funkcje.h"
#include "SM2025-Paleta.h"
#include <iostream>

using namespace std;


//Czarny-Bialy - 6 bit
void paletaMedianCutBW6bit(WynikStruct* wynik)
{
    ileKubelkow=0;
    ileKolorow=0;
    czyscPalete6bit();
    SDL_Color kolor;
    int szary=0,numer=0,indeks=0;

    for(int y=0;y<wysokosc/2;y++)
    {
        for(int x=0;x<szerokosc/2;x++)
        {
            kolor=getPixel(x,y);
            szary=kolor.r*0.299 + kolor.g*0.587 + kolor.b*0.114;
            obrazekS[numer]= szary;
            setPixel(x+szerokosc/2,y,szary,szary,szary);
            numer++;
        }
    }

    MedianCutBW6bit(0,numer-1,6);

    int index=0;
    for(int y=0;y<wysokosc/2;y++)
    {
        for(int x=0;x<szerokosc/2;x++)
        {
            szary=getPixel(x+szerokosc/2,y).r;
            indeks=znajdzSasiadaBW6bit(szary);

            // Zapisz indeks do struktury
            wynik->indeksy[index] = indeks;
            index++;

            setPixel(x+szerokosc/2, y, paleta6[indeks].r,paleta6[indeks].g, paleta6[indeks].b);
            obrazekIn[y * (szerokosc / 2) + x]=indeks;
        }
    }

      // Skopiuj paletê do struktury wyniku
    for (int i = 0; i < 64; i++) {
        wynik->paleta[i] = paleta6[i];
    }

    narysujPaletaBW6bit(0,220,paleta6);
    SDL_UpdateWindowSurface(window);
}

void narysujPaletaBW6bit(int px, int py, SDL_Color pal6b[])
{
    int x,y;

    for(int k=0; k<64; k++)
    {
        y=k/8;
        x=k%8;

        for(int xx=0;xx<40;xx++)
            for(int yy=0; yy<20;yy++)
                setPixel(x*40+xx+px, y*20+yy+py, pal6b[k].r,pal6b[k].g,pal6b[k].b);
    }
}

void czyscPalete6bit()
{
    for (int k=0; k<64; k++)
        paleta6[k] = {0, 0, 0};

    ileKolorow = 0;


    for(int i=0;i<wysokosc/2*szerokosc/2;i++)
        obrazekIn[i]=0;
}

void sortujKubelekBWQuick6bit(int start, int koniec)
{
    if (start >= koniec) return;

    int pivot = obrazekS[koniec];
    int i = start - 1;

    for (int j = start; j < koniec; j++)
    {
        if (obrazekS[j] <= pivot)
        {
            i++;
            std::swap(obrazekS[i], obrazekS[j]);
        }
    }
    std::swap(obrazekS[i + 1], obrazekS[koniec]);

    sortujKubelekBWQuick6bit(start, i);
    sortujKubelekBWQuick6bit(i + 2, koniec);
}


void MedianCutBW6bit(int start, int koniec, int iteracja)
{
    if (start > koniec)
        return;

    for(int i=0;i<iteracja;i++)
        cout<<" ";

    cout << "start: " << start << ", koniec: " << koniec << ", iteracja: " << iteracja << endl;

    if (iteracja>0)
    {
        sortujKubelekBWQuick6bit(start,koniec);
        for (int i=0; i<iteracja;i++)
            cout <<" ";

        cout << "Dzielimy kubelek na poziomie " << iteracja << endl;
        int srodek = (start+koniec+1)/2;
        MedianCutBW6bit(start,srodek-1,iteracja-1);
        MedianCutBW6bit(srodek,koniec,iteracja-1);
    }
    else
    {
        int sumaBW = 0;
        for(int p= start; p<=koniec; p++)
            sumaBW += obrazekS[p];

        Uint8 noweBW = sumaBW/ (koniec+1 - start);


            SDL_Color nowyKolor = {noweBW, noweBW, noweBW};
            paleta6[ileKubelkow] = nowyKolor;

            cout << "Kubelek " << ileKubelkow << ": ";
            cout << "(s:" << start << ", k:" << koniec << ", e:" << (koniec + 1 - start) << ")";
            cout << endl;
            cout << "Kolor " << ileKubelkow << " :(" << (int)nowyKolor.r << ", ";
            cout << (int)nowyKolor.g << "," << (int)nowyKolor.b << ")" << endl;

            ileKubelkow++;
    }
}

int znajdzSasiadaBW6bit(Uint8 wartosc)
{
    int minimum = 999;
    int indexMinimum = 0;
    int odleglosc = 0;

    for (int i = 0; i < ileKubelkow; i++)
        {
        odleglosc = abs(wartosc - paleta6[i].r);
        if (odleglosc < minimum)
        {
            minimum = odleglosc;
            indexMinimum = i;

            if (minimum == 0) break;
        }
    }
    return indexMinimum;
}

//Kolorowe - 6 bit
void paletaMedianCut6bit(WynikStruct* wynik)
{
    ileKubelkow = 0;
    ileKolorow = 0;
    czyscPalete6bit();

    SDL_Color kolor;
    int numer = 0;
    int indeks = 0;


    for (int y = 0; y < wysokosc / 2; y++)
    {
        for (int x = 0; x < szerokosc / 2; x++)
        {
            kolor = getPixel(x, y);
            obrazekK[numer] = { kolor.r, kolor.g, kolor.b };

            numer++;
        }
    }

    MedianCut6bit(0, numer-1,6);

    int index=0;
    for(int y=0;y<wysokosc/2;y++)
    {
        for(int x=0;x<szerokosc/2;x++)
        {
            kolor=getPixel(x,y);
            indeks=znajdzSasiada6bit(kolor);

            // Zapisz indeks do struktury
            wynik->indeksy[index] = indeks;
            index++;

            setPixel(x + szerokosc/2, y,
                     paleta6[indeks].r,paleta6[indeks].g, paleta6[indeks].b);
             obrazekIn[y * (szerokosc / 2) + x]=indeks;
        }
    }

    for (int i = 0; i < 64; i++) {
        wynik->paleta[i] = paleta6[i];
    }

    narysujPaleta6bit(0,220, paleta6);
}

void MedianCut6bit(int start, int koniec, int iteracja)
{
    for(int i = 0; i < iteracja; i++)
    {
        cout << " ";
    }
    cout << "start: " << start << ", koniec: " << koniec << ", iteracja: " << iteracja << endl;

    if (iteracja > 0)
    {
        uint8_t sortowanie = najwiekszaRoznica6bit(start, koniec);
        switch(sortowanie)
        {
            case 1: cout<<"Sortujemy wedlug R"<<endl;
                break;
            case 2: cout<<"Sortujemy wedlug G"<<endl;
                break;
            case 3: cout<<"Sortujemy wedlug B"<<endl;
                break;
        }
        quickSort6bit(start, koniec, sortowanie);
        cout << "Posortowane"<<endl;

        for (int i = 0; i < iteracja; i++)
            cout << " ";

        cout << "Dzielimy kubek na poziomie " << iteracja << endl;

        int srodek = (start + koniec + 1) / 2;

        MedianCut6bit(start, srodek - 1, iteracja - 1);
        MedianCut6bit(srodek, koniec, iteracja - 1);
    }
    else
    {
        int sumaR = 0, sumaG = 0, sumaB = 0;
        for (int p = start; p <= koniec; p++)
        {
            sumaR += obrazekK[p].r;
            sumaG += obrazekK[p].g;
            sumaB += obrazekK[p].b;
        }

        int sredniaR = sumaR / (koniec + 1 - start);
        int sredniaG = sumaG / (koniec + 1 - start);
        int sredniaB = sumaB / (koniec + 1 - start);

        SDL_Color nowyKolor = {sredniaR, sredniaG, sredniaB};
        paleta6[ileKubelkow] = nowyKolor;

        cout << "Kubelek " << ileKubelkow << ": ";
        cout << "s:<" << start << ", k:<" << koniec << ", e:<" << (koniec + 1 - start) << ">" << endl;
        cout << endl;
        cout << "Kolor " << ileKubelkow << ": (" << (int)nowyKolor.r << "x";
        cout << (int)nowyKolor.g << "x" << (int)nowyKolor.b << ")" << endl;
        ileKubelkow++;
    }
}

uint8_t najwiekszaRoznica6bit(int start, int koniec)
{
    int minR = start, minG = start, minB = start;
    int maxR = start, maxG = start, maxB = start;

    uint8_t roznica = 0;

    for (int i = start; i <= koniec; i++)
    {
        if (obrazekK[i].b < obrazekK[minB].b) minB = i;
        if (obrazekK[i].g < obrazekK[minG].g) minG = i;
        if (obrazekK[i].r < obrazekK[minR].r) minR = i;

        if (obrazekK[i].b > obrazekK[maxB].b) maxB = i;
        if (obrazekK[i].g > obrazekK[maxG].g) maxG = i;
        if (obrazekK[i].r > obrazekK[maxR].r) maxR = i;
    }

    int roznicaR = obrazekK[maxR].r - obrazekK[minR].r;
    int roznicaG = obrazekK[maxG].g - obrazekK[minG].g;
    int roznicaB = obrazekK[maxB].b - obrazekK[minB].b;

    cout << "roznice: (R:" << roznicaR
         << ", G:" << roznicaG
         << ", B:" << roznicaB << ")" << endl;

    if (roznicaR >= roznicaG && roznicaR >= roznicaB)
        roznica = 1;
    else if (roznicaG >= roznicaR && roznicaG >= roznicaB)
        roznica = 2;
    else
        roznica = 3;

    return roznica;
}

void quickSort6bit(int start, int koniec, Uint8 sortowanie)
{
    if (start >= koniec) return;

    int pivotIndex = partitionx6bit(start, koniec, sortowanie);
    quickSort6bit(start, pivotIndex - 1, sortowanie);
    quickSort6bit(pivotIndex + 1, koniec, sortowanie);
}

int partitionx6bit(int start, int koniec, Uint8 sortowanie)
{
    int pivotIndex = koniec;
    int i = start - 1;

    for (int j = start; j < koniec; j++)
    {
        bool condition = false;
        switch (sortowanie)
        {
            case 1:
                condition = obrazekK[j].r < obrazekK[pivotIndex].r;
                break;
            case 2:
                condition = obrazekK[j].g < obrazekK[pivotIndex].g;
                break;
            case 3:
                condition = obrazekK[j].b < obrazekK[pivotIndex].b;
                break;
        }

        if (condition)
        {
            i++;
            swap(obrazekK[i], obrazekK[j]);
        }
    }
    swap(obrazekK[i + 1], obrazekK[pivotIndex]);

    return i + 1;
}

int znajdzSasiada6bit(SDL_Color kolor)
{
    int minimum = 999999;
    int indexMinimum;

    SDL_Color kolorPaleta;
    float odleglosc;

    for (int i = 0; i < 64; i++)
    {
        kolorPaleta = paleta6[i];
        odleglosc = sqrt((kolor.r - kolorPaleta.r) * (kolor.r - kolorPaleta.r) +
                         (kolor.g - kolorPaleta.g) * (kolor.g - kolorPaleta.g) +
                         (kolor.b - kolorPaleta.b) * (kolor.b - kolorPaleta.b));

        if (odleglosc < minimum)
        {
            minimum = odleglosc;
            indexMinimum = i;
        }
    }
    return indexMinimum;
}

void narysujPaleta6bit(int px, int py, SDL_Color pal6[])
{
    int x,y;
    for(int k=0; k<64; k++)
    {
        y=k/8;
        x=k%8;

        for(int xx=0;xx<40;xx++)
            for(int yy=0; yy<20;yy++)
                setPixel(x*40+xx+px, y*20+yy+py, pal6[k].r,pal6[k].g,pal6[k].b);
    }
}










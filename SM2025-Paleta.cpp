// funkcje do redukcji kolorów i tworzenia palet
#include "SM2025-Paleta.h"
#include "SM2025-Zmienne.h"
#include "SM2025-Funkcje.h"

Uint8 z24Kdo6K(SDL_Color kolor)
{
    Uint8 kolor6bit;
    int R, G, B;
    int nowyR, nowyG, nowyB;

    R = kolor.r;
    G = kolor.g;
    B = kolor.b;
    nowyR = round(R*3.0/255.0);
    nowyG = round(G*3.0/255.0);
    nowyB = round(B*3.0/255.0);

    kolor6bit = (nowyR<<4) | (nowyG<<2) | nowyB;

    return kolor6bit;
}

SDL_Color z6Kdo24K(Uint8 kolor6bit)
{
    SDL_Color kolor;
    int R, G, B;
    int nowyR, nowyG, nowyB;

    nowyR = (kolor6bit&(0b00110000))>>4;
    nowyG = (kolor6bit&(0b00001100))>>2;
    nowyB = (kolor6bit&(0b00000011));

    R = nowyR*255.0/3.0;
    G = nowyG*255.0/3.0;
    B = nowyB*255.0/3.0;

    kolor.r = R;
    kolor.g = G;
    kolor.b = B;

    return kolor;
}

//rysuje palete na podstawie zmiennej przechowujacej palete
void narysujPalete6BIT(int px, int py, SDL_Color pal6[])
{
    int x, y;
    for (int k=0; k<64; k++){
        y = k / 16;
        x = k % 16;

        for (int xx=0; xx<20; xx++){
            for (int yy=0; yy<20; yy++){
                setPixel(x*20+xx+px, y*20+yy+py, pal6[k].r, pal6[k].g, pal6[k].b);
            }
        }
    }
}


void szary()
{
    SDL_Color kolor;

    int szary;
    for(int y=0;y<wysokosc/2;y++)
    {
        for(int x=0;x<szerokosc/2;x++)
        {
            kolor=getPixel(x,y);

            szary=kolor.r*0.299+kolor.g*0.587+kolor.b*0.114;
            setPixel(x+szerokosc/2,y,szary,szary,szary);
        }
    }
}

Uint8 z24Kdo6Kszary(SDL_Color kolor)
{

    Uint8 szary6bit;
    int jasnosc;
    jasnosc=0.299*kolor.r+0.587*kolor.g+0.114*kolor.b;

    szary6bit=round(jasnosc*63.0/255.0);

    return szary6bit;
}

SDL_Color z6Kdo24Kszary(Uint8 szary6bit)
{
    SDL_Color szary;
    int jasnosc;

    jasnosc = round(szary6bit * 255.0/63.0);

    szary.r = jasnosc;
    szary.g = jasnosc;
    szary.b = jasnosc;

    return szary;
}

bool porownajKolory(SDL_Color kolor1, SDL_Color kolor2)
{
    if (kolor1.r != kolor2.r)
        return false;
    if (kolor1.g != kolor2.g)
        return false;
    if (kolor1.b != kolor2.b)
        return false;

    return true;
}


int dodajKolor6K(SDL_Color kolor)
{
    int aktualnyKolor = ileKolorow;
    paleta6[aktualnyKolor] = kolor;
    if (ileKolorow < 64){
        cout << aktualnyKolor <<": [";
        cout << (int)kolor.r << "," << (int)kolor.g << "," << (int)kolor.b;
        cout << "]" << endl;
    }
    ileKolorow++;
    return aktualnyKolor;
}

int sprawdzKolor6K(SDL_Color kolor)
{
    if (ileKolorow > 0){
        for (int k=0; k<ileKolorow; k++){
            if (porownajKolory(kolor, paleta6[k]))
                return k;
        }
    }
    return dodajKolor6K(kolor);
}

void czyscPalete6K()
{
    for (int k=0; k<ileKolorow; k++)
        paleta6[k] = {0, 0, 0};
    ileKolorow = 0;
}

void paletaWykryta6K()
{
    czyscPalete6K();
    int indexKoloru;
    SDL_Color kolor;
    for (int y=0; y<wysokosc/2; y++){
        for (int x=szerokosc/2; x<szerokosc; x++){
            kolor = getPixel(x, y);
            indexKoloru = sprawdzKolor6K(kolor);
        }
    }
    cout << endl << "Ile kolorow: " << ileKolorow << endl;
    if (ileKolorow <= 64){
        cout << "Paleta spelnia ograniczenia 6-bit / piksel" << endl;
        narysujPalete6BIT(0, 200, paleta6);
    }
    else
        cout << "Paleta nie spelnia ograniczen 6-bit / piksel" << endl;
}

// podstawowe funkcje
#include "SM2025-Funkcje.h"
#include "SM2025-Zmienne.h"
#include "SM2025-Paleta.h"
#include "SM2025-MedianCut.h"
#include "SM2025-Pliki.h"
#include <iomanip>
#include <string>
#include <fstream>
const int pixel_count = hheight*hwidth;

void Funkcja1() {

    SDL_UpdateWindowSurface (window);
}

void Funkcja2() {

     SDL_UpdateWindowSurface(window);


}
void Funkcja3() {

    SDL_UpdateWindowSurface(window);
}
void Funkcja4() {

        SDL_UpdateWindowSurface(window);

}

void Funkcja5() {


    SDL_UpdateWindowSurface(window);
}



void Funkcja6() {
   float kolor = 0;
   for(int y = 0; y<rozmiarBloku; y++){
        for(int x = 0; x < rozmiarBloku ; x++){
            setPixel(x+rozmiarBloku, y + rozmiarBloku, kolor, kolor, kolor);
            kolor += 256.0/(rozmiarBloku*rozmiarBloku);
        }
   }
   for(int y = 1; y<rozmiarBloku; y+=2){
        for(int x = 1; x < rozmiarBloku; x+=2){
            setPixel(x+3*rozmiarBloku, y+rozmiarBloku, 255,255,255);
            setPixel(x+3*rozmiarBloku-1, y+rozmiarBloku-1, 128,128,128);
            setPixel(x+3*rozmiarBloku, y+rozmiarBloku-1, 0,0,0);
            setPixel(x+3*rozmiarBloku-1, y+rozmiarBloku, 0,0,0);
        }
   }
   SDL_UpdateWindowSurface(window);
}

void Funkcja7() {
    macierz blokDCT;
    macierz blokDane;
    macierz noweDane;
    //Pierwszy blok
    cout<<"Pierwszy blok"<<endl;
    for(int y = 0; y < rozmiarBloku; y++){
        for(int x = 0; x < rozmiarBloku; x++){
            blokDane.dane[x][y] = getPixel(x+rozmiarBloku, y+rozmiarBloku).r;
            blokDane.dct[x][y] = 0;
        }
    }
    wyswietlDane(blokDane);
    cout<<endl;
    blokDCT = dct(blokDane.dane);
    blokDCT.dct[2][2]=200;
    wyswietlDCT(blokDCT);
    cout<<endl;
    noweDane = idct(blokDane.dct);
    wyswietlDCT(noweDane);
    cout<<endl;
    //Drugi blok
    cout<<"Drugi blok"<<endl;
    for(int y = 0; y < rozmiarBloku; y++){
        for(int x = 0; x < rozmiarBloku; x++){
            blokDane.dane[x][y] = getPixel(x+3*rozmiarBloku, y+rozmiarBloku).r;
            blokDane.dct[x][y]=0;
        }
    }
    wyswietlDane(blokDane);
    cout<<endl;
    blokDCT = dct(blokDane.dane);
    blokDCT.dct[2][2]=200;
    wyswietlDCT(blokDCT);
    cout<<endl;
    noweDane = idct(blokDane.dct);
    wyswietlDCT(noweDane);
    cout<<endl;
    SDL_UpdateWindowSurface(window);
}

void Funkcja8() {
    SDL_UpdateWindowSurface(window);
}

void Funkcja9() {
    kompresjaDCT();
}



void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B)
{
  if ((x>=0) && (x<szerokosc) && (y>=0) && (y<wysokosc))
  {
    /* Zamieniamy poszczególne składowe koloru na format koloru piksela */
    Uint32 pixel = SDL_MapRGB(screen->format, R, G, B);

    /* Pobieramy informację ile bajtów zajmuje jeden piksel */
    int bpp = screen->format->BytesPerPixel;

    /* Obliczamy adres piksela */
    Uint8 *p1 = (Uint8 *)screen->pixels + (y*2) * screen->pitch + (x*2) * bpp;
    Uint8 *p2 = (Uint8 *)screen->pixels + (y*2+1) * screen->pitch + (x*2) * bpp;
    Uint8 *p3 = (Uint8 *)screen->pixels + (y*2) * screen->pitch + (x*2+1) * bpp;
    Uint8 *p4 = (Uint8 *)screen->pixels + (y*2+1) * screen->pitch + (x*2+1) * bpp;

    /* Ustawiamy wartość piksela, w zależnoœci od formatu powierzchni*/
    switch(bpp)
    {
        case 1: //8-bit
            *p1 = pixel;
            *p2 = pixel;
            *p3 = pixel;
            *p4 = pixel;
            break;

        case 2: //16-bit
            *(Uint16 *)p1 = pixel;
            *(Uint16 *)p2 = pixel;
            *(Uint16 *)p3 = pixel;
            *(Uint16 *)p4 = pixel;
            break;

        case 3: //24-bit
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p1[0] = (pixel >> 16) & 0xff;
                p1[1] = (pixel >> 8) & 0xff;
                p1[2] = pixel & 0xff;
                p2[0] = (pixel >> 16) & 0xff;
                p2[1] = (pixel >> 8) & 0xff;
                p2[2] = pixel & 0xff;
                p3[0] = (pixel >> 16) & 0xff;
                p3[1] = (pixel >> 8) & 0xff;
                p3[2] = pixel & 0xff;
                p4[0] = (pixel >> 16) & 0xff;
                p4[1] = (pixel >> 8) & 0xff;
                p4[2] = pixel & 0xff;
            } else {
                p1[0] = pixel & 0xff;
                p1[1] = (pixel >> 8) & 0xff;
                p1[2] = (pixel >> 16) & 0xff;
                p2[0] = pixel & 0xff;
                p2[1] = (pixel >> 8) & 0xff;
                p2[2] = (pixel >> 16) & 0xff;
                p3[0] = pixel & 0xff;
                p3[1] = (pixel >> 8) & 0xff;
                p3[2] = (pixel >> 16) & 0xff;
                p4[0] = pixel & 0xff;
                p4[1] = (pixel >> 8) & 0xff;
                p4[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4: //32-bit
            *(Uint32 *)p1 = pixel;
            *(Uint32 *)p2 = pixel;
            *(Uint32 *)p3 = pixel;
            *(Uint32 *)p4 = pixel;
            break;

        }
    }
}

void setPixelSurface(int x, int y, Uint8 R, Uint8 G, Uint8 B)
{
  if ((x>=0) && (x<szerokosc*2) && (y>=0) && (y<wysokosc*2))
  {
    /* Zamieniamy poszczególne składowe koloru na format koloru piksela */
    Uint32 pixel = SDL_MapRGB(screen->format, R, G, B);

    /* Pobieramy informację ile bajtów zajmuje jeden piksel */
    int bpp = screen->format->BytesPerPixel;

    /* Obliczamy adres piksela */
    Uint8 *p = (Uint8 *)screen->pixels + y * screen->pitch + x * bpp;

    /* Ustawiamy wartość piksela, w zależności od formatu powierzchni*/
    switch(bpp)
    {
        case 1: //8-bit
            *p = pixel;
            break;

        case 2: //16-bit
            *(Uint16 *)p = pixel;
            break;

        case 3: //24-bit
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4: //32-bit
            *(Uint32 *)p = pixel;
            break;
        }
    }
}

SDL_Color getPixel(int x, int y) {
    SDL_Color color ;
    Uint32 col = 0 ;
    if ((x>=0) && (x<szerokosc) && (y>=0) && (y<wysokosc)) {
        //określamy pozycję
        char* pPosition=(char*)screen->pixels ;

        //przesunięcie względem y
        pPosition+=(screen->pitch*y*2) ;

        //przesunięcie względem x
        pPosition+=(screen->format->BytesPerPixel*x*2);

        //kopiujemy dane piksela
        memcpy(&col, pPosition, screen->format->BytesPerPixel);

        //konwertujemy kolor
        SDL_GetRGB(col, screen->format, &color.r, &color.g, &color.b);
    }
    return ( color ) ;
}

SDL_Color getPixelSurface(int x, int y, SDL_Surface *surface) {
    SDL_Color color ;
    Uint32 col = 0 ;
    if ((x>=0) && (x<szerokosc) && (y>=0) && (y<wysokosc)) {
        //określamy pozycję
        char* pPosition=(char*)surface->pixels ;

        //przesunięcie względem y
        pPosition+=(surface->pitch*y) ;

        //przesunięcie względem x
        pPosition+=(surface->format->BytesPerPixel*x);

        //kopiujemy dane piksela
        memcpy(&col, pPosition, surface->format->BytesPerPixel);

        //konwertujemy kolor
        SDL_GetRGB(col, surface->format, &color.r, &color.g, &color.b);
    }
    return ( color ) ;
}


void ladujBMP(char const* nazwa, int x, int y) {
    SDL_Surface* bmp = SDL_LoadBMP(nazwa);
    if (!bmp)
    {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
    }

    else
    {
        SDL_Color kolor;
        for (int yy=0; yy<bmp->h; yy++) {
			for (int xx=0; xx<bmp->w; xx++) {
				kolor = getPixelSurface(xx, yy, bmp);
				setPixel(xx, yy, kolor.r, kolor.g, kolor.b);
			}
        }
		SDL_FreeSurface(bmp);
        SDL_UpdateWindowSurface(window);
    }

}


void czyscEkran(Uint8 R, Uint8 G, Uint8 B)
{
    SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, R, G, B));
    SDL_UpdateWindowSurface(window);
}


void rysujZStrukturyWynik(WynikStruct* wynik, int offsetX, int offsetY)
{
    int index = 0;
    SDL_Color kolor;

    for (int y = 0; y < wysokosc/2; y++)
    {
        for (int x = 0; x < szerokosc/2; x++)
        {
            // Pobierz indeks koloru dla tego piksela
            Uint8 indeksKoloru = wynik->indeksy[index];

            // Pobierz kolor z palety
            kolor = wynik->paleta[indeksKoloru];

            // Narysuj piksel z przesunięciem
            setPixel(x + offsetX, y + offsetY, kolor.r, kolor.g, kolor.b);

            index++;
        }
    }

    SDL_UpdateWindowSurface(window);
}

void zapiszCwiartkeJakoBMP(string nazwa) {
    SDL_Surface* screenSurf = SDL_GetWindowSurface(window);

    // Ustawiamy maski kolorów (bezpieczne dla Windows/Linux)
    Uint32 rmask, gmask, bmask, amask;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000; gmask = 0x00ff0000; bmask = 0x0000ff00; amask = 0x000000ff;
    #else
        rmask = 0x00ff0000; gmask = 0x0000ff00; bmask = 0x000000ff; amask = 0xff000000;
    #endif

    // 1. CEL (Destination): Tworzymy małą powierzchnię (320x200)
    // To jest rozmiar pliku, jaki chcemy uzyskać.
    int targetW = 320; // lub hwidth
    int targetH = 200; // lub hheight

    SDL_Surface* tempSurf = SDL_CreateRGBSurface(0, targetW, targetH, 32, rmask, gmask, bmask, amask);
    if (!tempSurf) {
        cout << "[BLAD] Nie udalo sie utworzyc powierzchni BMP: " << SDL_GetError() << endl;
        return;
    }

    // 2. ŹRÓDŁO (Source): Co pobieramy z ekranu?
    // Skoro obraz zajmuje u Ciebie obszar 640x400 (szerokosc x wysokosc), pobieramy całość tego obszaru.
    SDL_Rect srcRect;
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = szerokosc; // 640 - pobieramy duży obszar
    srcRect.h = wysokosc;  // 400

    // 3. CEL (Destination Rect): Gdzie to wklejamy?
    // Wklejamy do małej powierzchni, wypełniając ją całą.
    SDL_Rect dstRect;
    dstRect.x = 0;
    dstRect.y = 0;
    dstRect.w = targetW; // 320 - skalujemy do małego
    dstRect.h = targetH; // 200

    // 4. SKALOWANIE (Kluczowa zmiana: BlitScaled zamiast BlitSurface)
    // SDL_BlitScaled automatycznie zmniejszy obraz z 640x400 do 320x200
    if (SDL_BlitScaled(screenSurf, &srcRect, tempSurf, &dstRect) != 0) {
         cout << "[BLAD] Skalowanie nie powiodlo sie: " << SDL_GetError() << endl;
    } else {
        if (SDL_SaveBMP(tempSurf, nazwa.c_str()) != 0) {
             cout << "[BLAD] Zapisu BMP: " << SDL_GetError() << endl;
        } else {
             cout << "[SUKCES] Zapisano przeskalowany BMP: " << nazwa << endl;
        }
    }

    SDL_FreeSurface(tempSurf);
}

string getOpisTrybu(uint8_t t) {
    switch(t) {
        case 0: return "RGB555 (16-bit)";
        case 1: return "RGB888 (24-bit)";
        case 2: return "YCbCr888 (24-bit)";
        case 3: return "RGB555 + Dithering";
        case 4: return "Skala Szarosci RGB";
        case 5: return "Skala Szarosci YCbCr";
        default: return "Nieznany";
    }
}

string getOpisKompresji(uint8_t k) {
    switch(k) {
        case 0: return "Brak (RAW)";
        case 1: return "Bezstratna (RLE)";
        case 2: return "Stratna (DCT)";
        default: return "Nieznana";
    }
}





void przetworzIZapiszBlok(std::ofstream& plik, Uint8* bufor, int width, int height, int x, int y) {
    Uint8 blokWe[rozmiarBloku][rozmiarBloku];
    for(int by = 0; by < rozmiarBloku; by++) {
        for(int bx = 0; bx < rozmiarBloku; bx++) {
            if (x + bx < width && y + by < height) {
                blokWe[bx][by] = bufor[(y + by) * width + (x + bx)];
            } else {
                blokWe[bx][by] = 0;
            }
        }
    }

    macierz poDCT = dct(blokWe);
    float zigzag[rozmiarBloku * rozmiarBloku];
    zigzagCollect(poDCT.dct, zigzag);

    int ileWspolczynnikow = 15;
    for(int k = 0; k < ileWspolczynnikow; k++) {
        int wartosc = (int)(zigzag[k] / 2.0f); // Kwantyzacja / 2
        if(wartosc > 127) wartosc = 127;
        if(wartosc < -128) wartosc = -128;
        char b = (char)wartosc;
        plik.write(&b, 1);
    }
}

void odczytajIPrzetworzBlok(std::ifstream& plik, Uint8* bufor, int width, int height, int x, int y) {
    float zigzag[rozmiarBloku * rozmiarBloku];
    for(int k=0; k < rozmiarBloku * rozmiarBloku; k++) zigzag[k] = 0.0f;

    int ileWspolczynnikow = 15;
    for(int k = 0; k < ileWspolczynnikow; k++) {
        char b;
        plik.read(&b, 1);
        zigzag[k] = (float)((int)b * 2.0f); // Dekwantyzacja * 2
    }

    macierz poIDCT;
    zigzagReconstruct(zigzag, poIDCT.dct);
    macierz wynik = idct(poIDCT.dct);

    for(int by = 0; by < rozmiarBloku; by++) {
        for(int bx = 0; bx < rozmiarBloku; bx++) {
            if (x + bx < width && y + by < height) {
                bufor[(y + by) * width + (x + bx)] = wynik.dane[bx][by];
            }
        }
    }
}


void zapiszDG24(string nazwaPliku, uint8_t tryb, uint8_t predykcja, uint8_t kompresja) {
    cout << "\n[ZAPIS] " << nazwaPliku << endl;
    ofstream plik(nazwaPliku, ios::binary);
    if (!plik) { cout << "Blad pliku!" << endl; return; }

    plik.write("DG24", 4);
    uint16_t w = (uint16_t)hwidth;
    uint16_t h = (uint16_t)hheight;
    plik.write((char*)&w, 2);
    plik.write((char*)&h, 2);
    plik.write((char*)&tryb, 1);
    plik.write((char*)&predykcja, 1);
    plik.write((char*)&kompresja, 1);

    uint32_t placeholder = 0;
    streampos posSize = plik.tellp();
    plik.write((char*)&placeholder, 4);

    if (kompresja == 2) {
        cout << "[DCT] Subsampling 4:2:0 -> DCT..." << endl;

        int w2 = hwidth / 2;
        int h2 = hheight / 2;

        DaneKompresji daneKom;
        daneKom.Y  = new Uint8[hwidth * hheight];
        daneKom.Cb = new Uint8[w2 * h2];
        daneKom.Cr = new Uint8[w2 * h2];

        // 1. Subsampling do tablicy globalnej subcolors i wyswietlenie po prawej
        subsample420_YCbCr(hwidth, hheight);
        SDL_UpdateWindowSurface(window);

        // 2. Przepisanie z tablicy globalnej subcolors do buforow DCT
        // Zgodnie z poleceniem: mamy juz wartosci po subsamplingu w tablicy
        for (int y = 0; y < hheight; y++) {
            for (int x = 0; x < hwidth; x++) {
                // Y (c1) bierzemy dla każdego piksela
                daneKom.Y[y * hwidth + x] = (Uint8)subcolors[x][y].c1;
            }
        }

        // Cb (c2) i Cr (c3) bierzemy tylko dla pikseli co 2 (zgodnie z 4:2:0)
        for (int y = 0; y < h2; y++) {
            for (int x = 0; x < w2; x++) {
                // Mapujemy małe współrzędne (x,y) na duże (2x, 2y) z tablicy subcolors
                daneKom.Cb[y * w2 + x] = (Uint8)subcolors[x*2][y*2].c2;
                daneKom.Cr[y * w2 + x] = (Uint8)subcolors[x*2][y*2].c3;
            }
        }

        streampos startData = plik.tellp();

        // 3. ZAPIS BLOKÓW DCT
        // Y (Pełny rozmiar)
        for(int y = 0; y < hheight; y += rozmiarBloku) {
            for(int x = 0; x < hwidth; x += rozmiarBloku) {
                przetworzIZapiszBlok(plik, daneKom.Y, hwidth, hheight, x, y);
            }
        }
        // Cb (Połówkowy)
        for(int y = 0; y < h2; y += rozmiarBloku) {
            for(int x = 0; x < w2; x += rozmiarBloku) {
                przetworzIZapiszBlok(plik, daneKom.Cb, w2, h2, x, y);
            }
        }
        // Cr (Połówkowy)
        for(int y = 0; y < h2; y += rozmiarBloku) {
            for(int x = 0; x < w2; x += rozmiarBloku) {
                przetworzIZapiszBlok(plik, daneKom.Cr, w2, h2, x, y);
            }
        }

        streampos endData = plik.tellp();
        uint32_t size = (uint32_t)(endData - startData);
        plik.seekp(posSize);
        plik.write((char*)&size, 4);

        delete[] daneKom.Y; delete[] daneKom.Cb; delete[] daneKom.Cr;
        plik.close();
        cout << "[SUKCES] Rozmiar: " << size << endl;
        return;
    }

    if (predykcja == 1) {
        if (tryb == 0 || tryb == 3) filtrujRGB555_Typ3();
        else if (tryb == 1 || tryb == 4) filtrujPNG_Typ3();
        else if (tryb == 2 || tryb == 5) filtrujYCbCr_Typ3(tryb == 5);
    }

    int count = hwidth * hheight;
    Kolor* dane = new Kolor[count];

    for (int y = 0; y < hheight; y++) {
        for (int x = 0; x < hwidth; x++) {
            int i = y * hwidth + x;
            if (predykcja == 1) {
                if (tryb == 0 || tryb == 3) {
                    dane[i].c1 = filtrRGB555_lo[x][y];
                    dane[i].c2 = filtrRGB555_hi[x][y];
                    dane[i].c3 = 0;
                } else if (tryb == 1 || tryb == 4) {
                    dane[i].c1 = filtrPNG[x][y].r;
                    dane[i].c2 = filtrPNG[x][y].g;
                    dane[i].c3 = filtrPNG[x][y].b;
                } else if (tryb == 2 || tryb == 5) {
                    dane[i].c1 = (Uint8)filtrYCbCr[x][y].y;
                    dane[i].c2 = (Uint8)filtrYCbCr[x][y].cb;
                    dane[i].c3 = (Uint8)filtrYCbCr[x][y].cr;
                }
            } else {
                if (tryb == 0) {
                    Uint16 px = getRGB555_(x, y);
                    dane[i].c1 = px & 0xFF; dane[i].c2 = (px >> 8) & 0xFF; dane[i].c3 = 0;
                } else if (tryb == 3) {
                    Uint16 px = getRGB555D_(x, y);
                    dane[i].c1 = px & 0xFF; dane[i].c2 = (px >> 8) & 0xFF; dane[i].c3 = 0;
                } else if (tryb == 1) {
                    SDL_Color px = getPixel(x, y);
                    dane[i].c1 = px.r; dane[i].c2 = px.g; dane[i].c3 = px.b;
                } else if (tryb == 4) {
                    SDL_Color px = getPixel(x, y);
                    Uint8 szary = (Uint8)(0.299*px.r + 0.587*px.g + 0.114*px.b);
                    dane[i].c1 = szary; dane[i].c2 = szary; dane[i].c3 = szary;
                } else if (tryb == 2) {
                    YCbCr px = getYCbCr(x, y);
                    dane[i].c1 = (Uint8)px.y; dane[i].c2 = (Uint8)px.cb; dane[i].c3 = (Uint8)px.cr;
                } else if (tryb == 5) {
                    YCbCr px = getYCbCr(x, y);
                    dane[i].c1 = (Uint8)px.y; dane[i].c2 = 128; dane[i].c3 = 128;
                }
            }
        }
    }

    streampos startData = plik.tellp();
    if (kompresja == 1) {
        RLEColors rle = kompresjaObrazu_RLE(dane, count);
        zapiszRLEdoStrumienia(plik, &rle);
        delete rle.rtab; delete rle.gtab; delete rle.btab;
    } else {
        for(int i=0; i<count; i++) {
            char b[3] = {(char)dane[i].c1, (char)dane[i].c2, (char)dane[i].c3};
            if(tryb == 0 || tryb == 3) plik.write(b, 2);
            else plik.write(b, 3);
        }
    }
    streampos endData = plik.tellp();
    uint32_t size = (uint32_t)(endData - startData);
    plik.seekp(posSize);
    plik.write((char*)&size, 4);
    plik.close();
    delete[] dane;
    cout << "[SUKCES] Rozmiar: " << size << endl;
}

void wczytajDG24(string nazwaPliku) {
    cout << "\n[ODCZYT] " << nazwaPliku << endl;
    ifstream plik(nazwaPliku, ios::binary);
    if (!plik) { cout << "Blad pliku!" << endl; return; }

    char head[4];
    uint16_t w, h;
    uint8_t tryb, pred, komp;
    uint32_t rozmiar;

    plik.read(head, 4);
    if (string(head,4) != "DG24") { cout << "Zly format" << endl; return; }
    plik.read((char*)&w, 2); plik.read((char*)&h, 2);
    plik.read((char*)&tryb, 1); plik.read((char*)&pred, 1);
    plik.read((char*)&komp, 1); plik.read((char*)&rozmiar, 4);

    cout << "[INFO] " << w << "x" << h << " Tryb:" << getOpisTrybu(tryb) << " Komp:" << getOpisKompresji(komp) << endl;

    if (komp == 2) {
        cout << "[DCT] Dekompresja..." << endl;

        int w2 = hwidth / 2;
        int h2 = hheight / 2;

        DaneKompresji daneKom;
        daneKom.Y  = new Uint8[hwidth * hheight];
        daneKom.Cb = new Uint8[w2 * h2];
        daneKom.Cr = new Uint8[w2 * h2];

        // 1. ODCZYT I DEKWANTYZACJA DCT
        for(int y=0; y<hheight; y+=rozmiarBloku) {
            for(int x=0; x<hwidth; x+=rozmiarBloku) {
                odczytajIPrzetworzBlok(plik, daneKom.Y, hwidth, hheight, x, y);
            }
        }
        for(int y=0; y<h2; y+=rozmiarBloku) {
            for(int x=0; x<w2; x+=rozmiarBloku) {
                odczytajIPrzetworzBlok(plik, daneKom.Cb, w2, h2, x, y);
            }
        }
        for(int y=0; y<h2; y+=rozmiarBloku) {
            for(int x=0; x<w2; x+=rozmiarBloku) {
                odczytajIPrzetworzBlok(plik, daneKom.Cr, w2, h2, x, y);
            }
        }

        // 2. WYŚWIETLANIE (LEWA STRONA) - REKONSTRUKCJA 4:2:0
        // Dokładnie jak chciałeś: dla każdego piksela pobieramy wartości i używamy setYCbCr

        for (int y = 0; y < hheight; y++) {
            for (int x = 0; x < hwidth; x++) {
                // Y (Luma) bierzemy dla każdego piksela (pełna rozdzielczość)
                float yVal = (float)daneKom.Y[y * hwidth + x];

                // Cb i Cr bierzemy z tablic pomniejszonych (dzielimy współrzędne przez 2)
                float cbVal = (float)daneKom.Cb[(y / 2) * w2 + (x / 2)];
                float crVal = (float)daneKom.Cr[(y / 2) * w2 + (x / 2)];

                setYCbCr(x, y, yVal, cbVal, crVal);
            }
        }

        delete[] daneKom.Y; delete[] daneKom.Cb; delete[] daneKom.Cr;
        plik.close();
        SDL_UpdateWindowSurface(window);
        cout << "[SUKCES] Wczytano DCT." << endl;
        return;
    }

    // --- POZOSTAŁE TRYBY (Bez zmian) ---
    // ... reszta kodu wczytajDG24 ...
    int count = w * h;
    Kolor* dane = nullptr;

    if (komp == 1) { // RLE
        RLE* r = new RLE(nullptr, 0);
        RLE* l = new RLE(nullptr, 0);
        RLE* e = new RLE(nullptr, 0);
        RLEColors rle(r, l, e);
        wczytajPojedynczyRLE(plik, rle.rtab);
        wczytajPojedynczyRLE(plik, rle.gtab);
        wczytajPojedynczyRLE(plik, rle.btab);
        dane = dekompresjaObrazu_RLE(&rle);
        delete rle.rtab; delete rle.gtab; delete rle.btab;
    } else { // RAW
        dane = new Kolor[count];
        for(int i=0; i<count; i++) {
            char b;
            plik.read(&b, 1); dane[i].c1 = (Uint8)b;
            plik.read(&b, 1); dane[i].c2 = (Uint8)b;
            if (tryb != 0 && tryb != 3) {
                plik.read(&b, 1); dane[i].c3 = (Uint8)b;
            } else dane[i].c3 = 0;
        }
    }
    plik.close();

    if (pred == 1) {
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                int i = y * w + x;
                if (tryb == 0 || tryb == 3) {
                    filtrRGB555_lo[x][y] = dane[i].c1;
                    filtrRGB555_hi[x][y] = dane[i].c2;
                } else if (tryb == 1 || tryb == 4) {
                    filtrPNG[x][y].r = dane[i].c1;
                    filtrPNG[x][y].g = dane[i].c2;
                    filtrPNG[x][y].b = dane[i].c3;
                } else if (tryb == 2 || tryb == 5) {
                    filtrYCbCr[x][y].y = (float)dane[i].c1;
                    filtrYCbCr[x][y].cb = (float)dane[i].c2;
                    filtrYCbCr[x][y].cr = (float)dane[i].c3;
                }
            }
        }

        if (tryb == 2 || tryb == 5) {
            for (int y = 0; y < h; y++) {
                for (int x = 0; x < w; x++) {
                    YCbCr diff = filtrYCbCr[x][y];
                    YCbCr left = (x > 0) ? buforYCbCr[x - 1][y] : YCbCr{0, 0, 0};
                    YCbCr up   = (y > 0) ? buforYCbCr[x][y - 1] : YCbCr{0, 0, 0};
                    float predY  = floor((left.y + up.y) / 2.0f);
                    float predCb = floor((left.cb + up.cb) / 2.0f);
                    float predCr = floor((left.cr + up.cr) / 2.0f);
                    YCbCr rec;
                    rec.y  = diff.y + predY;
                    rec.cb = diff.cb + predCb;
                    rec.cr = diff.cr + predCr;
                    buforYCbCr[x][y] = rec;
                    setYCbCr(x, y, rec.y, rec.cb, rec.cr);
                }
            }
        }
        else if (tryb == 0 || tryb == 3) odfiltrujRGB555_Typ3();
        else if (tryb == 1 || tryb == 4) odfiltrujPNG_Typ3();

    } else {
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                int i = y * w + x;
                int tx = x;
                if (tryb == 0 || tryb == 3) {
                    Uint16 px = (Uint16)dane[i].c1 | ((Uint16)dane[i].c2 << 8);
                    setRGB555(tx, y, px);
                } else if (tryb == 1 || tryb == 4) {
                    setPixel(tx, y, dane[i].c1, dane[i].c2, dane[i].c3);
                } else if (tryb == 2 || tryb == 5) {
                    setYCbCr(tx, y, (float)dane[i].c1, (float)dane[i].c2, (float)dane[i].c3);
                }
            }
        }
    }
    delete[] dane;
    SDL_UpdateWindowSurface(window);
    cout << "[SUKCES] Obraz wczytany." << endl;
}

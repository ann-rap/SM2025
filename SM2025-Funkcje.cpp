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

// Globalne zmienne do przechowywania wyników
WynikStruct wynik1, wynik2, wynik3, wynik4, wynik5;
void Funkcja1() {//kompresja i dekompresja z tablicy
    //P6
    SDL_Color* colors = new SDL_Color[wysokosc/2 * szerokosc/2];
    for(int y = 0; y < wysokosc/2; y++) {
        for(int x = 0; x < szerokosc/2; x++) {
            colors[y * (szerokosc/2) + x] = getPixel(x, y);
        }
    }

    ByteRunColors komp = kompresjaObrazu(colors, (wysokosc/2) * (szerokosc/2));
    SDL_Color* dekomp = dekompresjObrazu(&komp);

    for(int y = 0; y < wysokosc/2; y++) {
        for(int x = 0; x < szerokosc/2; x++) {
            SDL_Color kolor = dekomp[y * (szerokosc/2) + x];
            setPixel(x + hwidth, y, kolor.r, kolor.g, kolor.b);
        }
    }

    SDL_UpdateWindowSurface (window);
}
////////Kompresja
void Funkcja2() {//zapis do pliku
    SDL_Color* colors = new SDL_Color[wysokosc/2 * szerokosc/2];
    for(int y = 0; y < wysokosc/2; y++) {
        for(int x = 0; x < szerokosc/2; x++) {
            colors[y * (szerokosc/2) + x] = getPixel(x, y);
        }
    }

    ByteRunColors komp = kompresjaObrazu(colors, (wysokosc/2) * (szerokosc/2));

    double procR = 100 - ((double)komp.rtab->len / pixel_count * 100.0);
    double procG = 100 - ((double)komp.gtab->len / pixel_count * 100.0);
    double procB = 100 - ((double)komp.btab->len / pixel_count * 100.0);

    long sumaSkompresowana = komp.rtab->len + komp.gtab->len + komp.btab->len;
    long sumaOryginalna = pixel_count * 3;
    double procTotal =100 - ((double)sumaSkompresowana / sumaOryginalna * 100.0);

    cout << "--- Kompresja ByteRun ---" << endl;
    cout << fixed << setprecision(2);
    cout << "Kanal R: " << procR << "% (" << komp.rtab->len << " elementow)" << endl;
    cout << "Kanal G: " << procG << "% (" << komp.gtab->len << " elementow)" << endl;
    cout << "Kanal B: " << procB << "% (" << komp.btab->len << " elementow)" << endl;
    cout << "SUMARYCZNIE: " << procTotal << "%" << endl<<endl;

    zapisz(&komp);
    SDL_Color* dekomp = dekompresjObrazu(&komp);

    for(int y = 0; y < wysokosc/2; y++) {
        for(int x = 0; x < szerokosc/2; x++) {
            SDL_Color kolor = dekomp[y * (szerokosc/2) + x];
            setPixel(x + hwidth, y, kolor.r, kolor.g, kolor.b);
        }
    }
     SDL_UpdateWindowSurface(window);


}
/////////dekompresja
void Funkcja3() {//odczyt z pliku
    ByteRun* r = new ByteRun(nullptr, 0);
    ByteRun* g = new ByteRun(nullptr, 0);
    ByteRun* b = new ByteRun(nullptr, 0);

    ByteRunColors* loaded = new ByteRunColors(r, g, b);
    wczytaj(loaded);
    SDL_Color* dekomp = dekompresjObrazu(loaded);

    for(int y = 0; y < wysokosc/2; y++) {
        for(int x = 0; x < szerokosc/2; x++) {
            SDL_Color kolor = dekomp[y * (szerokosc/2) + x];
            setPixel(x + hwidth, y+hheight, kolor.r, kolor.g, kolor.b);
        }
    }
    SDL_UpdateWindowSurface(window);
}
////////HSL------
void Funkcja4() {
         for(int y = 0; y<wysokosc/2;y++){
                 for(int x = 0; x <szerokosc/2;x++){
                    HSL nowyKolor = getHSL(x,y);
                    setHSL(x,y,nowyKolor.h,nowyKolor.s,nowyKolor.l);
                 }
            }
        SDL_UpdateWindowSurface(window);

}

void Funkcja5() {
    int w = szerokosc/2;
    int h = wysokosc/2;
    int len = w * h;

    SDL_Color* colors = new SDL_Color[len];

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            colors[y * w + x] = getPixel(x, y);
        }
    }

    Kolor* kolory = new Kolor[len];
    for (int i = 0; i < len; ++i) {
        kolory[i].c1 = colors[i].r;
        kolory[i].c2 = colors[i].g;
        kolory[i].c3 = colors[i].b;
    }

    LZWColors lzwColors = kompresjaObrazu_LZW(kolory, len);

    const char* filename = "dane_lzw.bin";
    zapisz_LZW(&lzwColors, filename);

    std::cout << "\nStopien kompresji: ";
    double oryg = len * 3.0;
    double komp = (lzwColors.rtab->len +
                   lzwColors.gtab->len +
                   lzwColors.btab->len) * 2.0;
    std::cout << (oryg / komp) << "\n";

    delete[] lzwColors.rtab->tab;
    delete   lzwColors.rtab;
    delete[] lzwColors.gtab->tab;
    delete   lzwColors.gtab;
    delete[] lzwColors.btab->tab;
    delete   lzwColors.btab;

    LZWColors lzwFromFile{};
    lzwFromFile.rtab = nullptr;
    lzwFromFile.gtab = nullptr;
    lzwFromFile.btab = nullptr;

    wczytaj_LZW(&lzwFromFile, filename);

    Kolor* zdekompresowane = dekompresjaObrazu_LZW(&lzwFromFile);

    bool ok = true;
    for (int i = 0; ok && i < len; ++i) {
        if (zdekompresowane[i].c1 != kolory[i].c1 ||
            zdekompresowane[i].c2 != kolory[i].c2 ||
            zdekompresowane[i].c3 != kolory[i].c3) {
            ok = false;
        }
    }

    std::cout << "\nCzy dekompresja po ZAPISIE DO PLIKU i ODCZYCIE jest poprawna? "
              << (ok ? "TAK" : "NIE") << "\n";

    delete[] colors;
    delete[] kolory;
    delete[] zdekompresowane;

    delete[] lzwFromFile.rtab->tab;
    delete   lzwFromFile.rtab;
    delete[] lzwFromFile.gtab->tab;
    delete   lzwFromFile.gtab;
    delete[] lzwFromFile.btab->tab;
    delete   lzwFromFile.btab;

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
    zapiszYUVDoBufora();
    filtrujYUV_Optymalnie(); odfiltrujYUV_Optymalnie();
    filtrujRGB555_Optymalnie(); odfiltrujRGB555_Optymalnie();
    filtrujRGB565_Optymalnie(); odfiltrujRGB565_Optymalnie();
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

// Rysuje obrazek z struktury WynikStruct
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

//do testowania RLE wywolanie przez klawisz "r"
void MenuRLE() {
    cout << "\n========================================" << endl;
    cout << "       KOMPRESJA RLE - MENU" << endl;
    cout << "========================================" << endl;
    cout << "1. RGB" << endl;
    cout << "2. HSL" << endl;
    cout << "3. YUV" << endl;
    cout << "4. YIQ" << endl;
    cout << "0. Anuluj" << endl;
    cout << "Wybierz opcje: ";

    int wybor;
    cin >> wybor;

    if (wybor == 0) {
        cout << "Anulowano." << endl;
        return;
    }

    Kolor* colors = new Kolor[pixel_count];


    // === POJEDYNCZE TESTY ===
    string format_name;
    string filename;

    // Przygotowanie danych w zależności od wyboru
    switch(wybor) {
        case 1: // RGB
            format_name = "RGB";
            for(int y = 0; y < wysokosc/2; y++) {
                for(int x = 0; x < szerokosc/2; x++) {
                    SDL_Color nk = getPixel(x, y);
                    colors[y * (szerokosc/2) + x].c1 = nk.r;
                    colors[y * (szerokosc/2) + x].c2 = nk.g;
                    colors[y * (szerokosc/2) + x].c3 = nk.b;
                }
            }
            break;

        case 2: // HSL
            format_name = "HSL";
            for(int y = 0; y < wysokosc/2; y++) {
                for(int x = 0; x < szerokosc/2; x++) {
                    HSL hsl = getHSL(x, y);
                    colors[y * (szerokosc/2) + x].c1 = (Uint8)((hsl.h / 360.0) * 255.0);
                    colors[y * (szerokosc/2) + x].c2 = (Uint8)(hsl.s * 255.0);
                    colors[y * (szerokosc/2) + x].c3 = (Uint8)(hsl.l * 255.0);
                }
            }
            break;

        case 3: // YUV
            format_name = "YUV";
            for(int y = 0; y < wysokosc/2; y++) {
                for(int x = 0; x < szerokosc/2; x++) {
                    YUV yuv = getYUV(x, y);
                    colors[y * (szerokosc/2) + x].c1 = (Uint8)yuv.y;
                    colors[y * (szerokosc/2) + x].c2 = (Uint8)(yuv.u + 128);
                    colors[y * (szerokosc/2) + x].c3 = (Uint8)(yuv.v + 128);
                }
            }
            break;

        case 4: // YIQ
            format_name = "YIQ";
            for(int y = 0; y < wysokosc/2; y++) {
                for(int x = 0; x < szerokosc/2; x++) {
                    YIQ yiq = getYIQ(x, y);
                    colors[y * (szerokosc/2) + x].c1 = (Uint8)yiq.y;
                    colors[y * (szerokosc/2) + x].c2 = (Uint8)((yiq.i + 0.5957) * 255.0 / 1.1914);
                    colors[y * (szerokosc/2) + x].c3 = (Uint8)((yiq.q + 0.5226) * 255.0 / 1.0452);
                }
            }
            break;

        default:
            cout << "Nieprawidlowy wybor!" << endl;
            delete[] colors;
            return;
    }

    // Kompresja
    cout << "\n--- Kompresja " << format_name << " ---" << endl;
    RLEColors komp = kompresjaObrazu_RLE(colors, pixel_count);

double procR = 100 - ((double)komp.rtab->len / pixel_count * 100.0);
    double procG = 100 - ((double)komp.gtab->len / pixel_count * 100.0);
    double procB = 100 - ((double)komp.btab->len / pixel_count * 100.0);

    long sumaSkompresowana = komp.rtab->len + komp.gtab->len + komp.btab->len;
    long sumaOryginalna = pixel_count * 3;
    double procTotal =100 - ((double)sumaSkompresowana / sumaOryginalna * 100.0);
    cout << fixed << setprecision(2);
    cout << "Kanal 1: " << procR <<endl;
    cout << "Kanal 2: " << procB<<endl;
    cout << "Kanal 3: "<< procG <<endl;
   // cout << "Rozmiar: " << suma << " (z " << (pixel_count * 3) << ")" << endl;
    cout << "Kompresja: " << procTotal << "%" << endl;

    // Zapis
    cout << "\nPodaj nazwe pliku do zapisu (bez rozszerzenia): ";
    cin >> filename;
    filename += ".z21";
    zapisz_RLE(&komp, filename.c_str());

    // Dekompresja i wyświetlenie
    cout << "Dekompresja i wyswietlanie..." << endl;
    Kolor* dekomp = dekompresjaObrazu_RLE(&komp);

    // Konwersja z powrotem do RGB i wyświetlenie
    switch(wybor) {
        case 1: // RGB - bezpośrednie
            for(int y = 0; y < wysokosc/2; y++) {
                for(int x = 0; x < szerokosc/2; x++) {
                    Kolor k = dekomp[y * (szerokosc/2) + x];
                    setPixel(x + hwidth, y, k.c1, k.c2, k.c3);
                }
            }
            break;

        case 2: // HSL -> RGB
            for(int y = 0; y < wysokosc/2; y++) {
                for(int x = 0; x < szerokosc/2; x++) {
                    Kolor k = dekomp[y * (szerokosc/2) + x];
                    float h = (k.c1 / 255.0) * 360.0;
                    float s = k.c2 / 255.0;
                    float l = k.c3 / 255.0;
                    setHSL(x + hwidth, y, h, s, l);
                }
            }
            break;

        case 3: // YUV -> RGB
            for(int y = 0; y < wysokosc/2; y++) {
                for(int x = 0; x < szerokosc/2; x++) {
                    Kolor k = dekomp[y * (szerokosc/2) + x];
                    YUV yuv;
                    yuv.y = k.c1;
                    yuv.u = k.c2 - 128;
                    yuv.v = k.c3 - 128;
                    setYUV(x + hwidth, y, yuv.y, yuv.u, yuv.v);
                }
            }
            break;

        case 4: // YIQ -> RGB
            for(int y = 0; y < wysokosc/2; y++) {
                for(int x = 0; x < szerokosc/2; x++) {
                    Kolor k = dekomp[y * (szerokosc/2) + x];
                    YIQ yiq;
                    yiq.y = k.c1;
                    yiq.i = (k.c2 / 255.0) * 1.1914 - 0.5957;
                    yiq.q = (k.c3 / 255.0) * 1.0452 - 0.5226;
                    setYIQ(x + hwidth, y, yiq.y, yiq.i, yiq.q);
                }
            }
            break;
    }

    delete[] colors;
    delete[] dekomp;

    SDL_UpdateWindowSurface(window);
    cout << "Gotowe!\n" << endl;

}
// Pomocnik do wyświetlania nazw trybów zgodnie z Tabelą 1.1
string getOpisTrybu(uint8_t t) {
    switch(t) {
        case 0: return "RGB555 (15-bit)";
        case 1: return "RGB888 (24-bit)";
        case 2: return "YCbCr888 (24-bit)";
        case 3: return "RGB555 + Dithering";
        case 4: return "Skala Szarosci RGB888";
        case 5: return "Skala Szarosci YCbCr888";
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


void zapiszDG24(string nazwaPliku, uint8_t tryb, uint8_t predykcja, uint8_t kompresja) {
    cout << "\n[ZAPIS] Start zapisu: " << nazwaPliku << endl;
    ofstream plik(nazwaPliku, ios::binary);
    if (!plik) { cout << "[BLAD] Brak dostepu do pliku!" << endl; return; }

    // --- 1. NAGŁÓWEK ---
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

    cout << "[INFO] Naglowek OK. Tryb: " << (int)tryb << endl;

    // --- 2. PREDYKCJA ---
    if (predykcja == 1) {
        cout << "[PROCES] Predykcja..." << endl;
        if (tryb == 0 || tryb == 3) {
            filtrujRGB555_Typ3(); // Zapisuje do filtrRGB555
        }
        else if (tryb == 1 || tryb == 4) { // RGB888 i Szary RGB
            filtrujPNG_Typ3();    // Zapisuje do filtrPNG
        }
        else if (tryb == 2 || tryb == 5) { // YCbCr i Szary YCbCr
            // Używamy nowej, dedykowanej funkcji (true jeśli tryb 5)
            filtrujYCbCr_Typ3(tryb == 5);
        }
    }

    // --- 3. ZBIERANIE DANYCH ---
    cout << "[PROCES] Agregacja danych..." << endl;
    int count = hwidth * hheight;
    Kolor* dane = new Kolor[count];

    for (int y = 0; y < hheight; y++) {
        for (int x = 0; x < hwidth; x++) {
            int i = y * hwidth + x;

            if (predykcja == 1) {
                // POBIERAMY Z TABLIC FILTRÓW
                if (tryb == 0 || tryb == 3) {
                    dane[i].c1 = filtrRGB555_lo[x][y];
                    dane[i].c2 = filtrRGB555_hi[x][y];
                    dane[i].c3 = 0;
                } else if (tryb == 1 || tryb == 4) {
                    dane[i].c1 = filtrPNG[x][y].r;
                    dane[i].c2 = filtrPNG[x][y].g;
                    dane[i].c3 = filtrPNG[x][y].b;
                } else if (tryb == 2 || tryb == 5) {
                    // Pobieramy z nowej tablicy filtrYCbCr
                    dane[i].c1 = (Uint8)filtrYCbCr[x][y].y;
                    dane[i].c2 = (Uint8)filtrYCbCr[x][y].cb;
                    dane[i].c3 = (Uint8)filtrYCbCr[x][y].cr;
                }
            } else {
                // POBIERAMY Z EKRANU (LEWA STRONA)
                if (tryb == 0) { // RGB555
                    Uint16 px = getRGB555_(x, y);
                    dane[i].c1 = px & 0xFF;
                    dane[i].c2 = (px >> 8) & 0xFF;
                    dane[i].c3 = 0;
                }
                else if (tryb == 3) { // RGB555 + Dither
                    Uint16 px = getRGB555D_(x, y);
                    dane[i].c1 = px & 0xFF;
                    dane[i].c2 = (px >> 8) & 0xFF;
                    dane[i].c3 = 0;
                }
                else if (tryb == 1) { // RGB888
                    SDL_Color px = getPixel(x, y);
                    dane[i].c1 = px.r; dane[i].c2 = px.g; dane[i].c3 = px.b;
                }
                else if (tryb == 4) { // RGB888 SZARY
                    SDL_Color px = getPixel(x, y);
                    Uint8 szary = (Uint8)(0.299*px.r + 0.587*px.g + 0.114*px.b);
                    dane[i].c1 = szary; dane[i].c2 = szary; dane[i].c3 = szary;
                }
                else if (tryb == 2) { // YCbCr888
                    YCbCr px = getYCbCr(x, y);
                    dane[i].c1 = (Uint8)px.y; dane[i].c2 = (Uint8)px.cb; dane[i].c3 = (Uint8)px.cr;
                }
                else if (tryb == 5) { // YCbCr888 SZARY
                    YCbCr px = getYCbCr(x, y);
                    dane[i].c1 = (Uint8)px.y;
                    dane[i].c2 = 128; // Zerujemy kolor
                    dane[i].c3 = 128; // Zerujemy kolor
                }
            }
        }
    }

    // --- 4. ZAPIS (RAW / RLE) ---
    streampos startData = plik.tellp();

    if (kompresja == 1) {
        cout << "[PROCES] Kompresja RLE i zapis..." << endl;
        RLEColors rle = kompresjaObrazu_RLE(dane, count);
        zapiszRLEdoStrumienia(plik, &rle);

        delete rle.rtab; delete rle.gtab; delete rle.btab;
    } else {
        cout << "[PROCES] Zapis RAW..." << endl;
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
    cout << "[SUKCES] Plik zapisany. Bajtow danych: " << size << endl;
}

void wczytajDG24(string nazwaPliku) {
    cout << "\n[ODCZYT] Start odczytu: " << nazwaPliku << endl;
    ifstream plik(nazwaPliku, ios::binary);
    if (!plik) { cout << "[BLAD] Plik nie istnieje!" << endl; return; }

    char head[4];
    uint16_t w, h;
    uint8_t tryb, pred, komp;
    uint32_t rozmiar;

    plik.read(head, 4);
    if (string(head,4) != "DG24") { cout << "[BLAD] Zly naglowek!" << endl; return; }

    plik.read((char*)&w, 2);
    plik.read((char*)&h, 2);
    plik.read((char*)&tryb, 1);
    plik.read((char*)&pred, 1);
    plik.read((char*)&komp, 1);
    plik.read((char*)&rozmiar, 4);

    cout << "[INFO] Format: " << w << "x" << h << " Tryb:" << (int)tryb
         << " Pred:" << (int)pred << " Komp:" << (int)komp << endl;

    int count = w * h;
    Kolor* dane = nullptr;

    // --- 1. ODCZYT/DEKOMPRESJA ---
    if (komp == 1) {
        cout << "[PROCES] Dekompresja RLE..." << endl;
        RLE* r = new RLE(nullptr, 0);
        RLE* l = new RLE(nullptr, 0);
        RLE* e = new RLE(nullptr, 0);
        RLEColors rle(r, l, e);

        wczytajPojedynczyRLE(plik, rle.rtab);
        wczytajPojedynczyRLE(plik, rle.gtab);
        wczytajPojedynczyRLE(plik, rle.btab);

        dane = dekompresjaObrazu_RLE(&rle);

        delete rle.rtab; delete rle.gtab; delete rle.btab;
    } else {
        cout << "[PROCES] Odczyt RAW..." << endl;
        dane = new Kolor[count];
        for(int i=0; i<count; i++) {
            char b;
            plik.read(&b, 1); dane[i].c1 = (Uint8)b;
            plik.read(&b, 1); dane[i].c2 = (Uint8)b;
            if (tryb != 0 && tryb != 3) {
                plik.read(&b, 1); dane[i].c3 = (Uint8)b;
            } else {
                dane[i].c3 = 0;
            }
        }
    }
    plik.close();

    // --- 2. REKONSTRUKCJA I WIZUALIZACJA (PRAWA STRONA) ---
    cout << "[PROCES] Wyswietlanie obrazu..." << endl;

    if (pred == 1) {
        // A. Wypełnienie tablic filtrów danymi z pliku
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
                    // Wpisujemy do nowej tablicy
                    filtrYCbCr[x][y].y  = (float)dane[i].c1;
                    filtrYCbCr[x][y].cb = (float)dane[i].c2;
                    filtrYCbCr[x][y].cr = (float)dane[i].c3;
                }
            }
        }

        // B. Odwrócenie predykcji i rysowanie
        // Upewnij się, że Twoje funkcje odfiltruj... też rysują na prawej stronie!
        // Jeśli nie, trzeba je zmodyfikować. Tutaj zakładam, że odfiltrujRGB555 i PNG to robią.
        // Dla YCbCr używamy nowej funkcji, która na 100% to robi.

        if (tryb == 0 || tryb == 3) {
            odfiltrujRGB555_Typ3(); // Rysuje w setRGB555
            // Jeśli ta funkcja nie przesuwa o hwidth, musisz to poprawić w definicji odfiltrujRGB!
        }
        else if (tryb == 1 || tryb == 4) {
            odfiltrujPNG_Typ3(); // Rysuje w setPixel
        }
        else if (tryb == 2 || tryb == 5) {
            odfiltrujYCbCr_Typ3(); // Ta funkcja już rysuje setYCbCr(x+hwidth, ...)
        }

    } else {
        // Bez predykcji - rysujemy bezpośrednio na prawej stronie
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                int i = y * w + x;
                int targetX = x + hwidth; // PRAWA STRONA

                if (tryb == 0 || tryb == 3) {
                    Uint16 px = (Uint16)dane[i].c1 | ((Uint16)dane[i].c2 << 8);
                    setRGB555(targetX, y, px);
                }
                else if (tryb == 1 || tryb == 4) { // RGB
                    setPixel(targetX, y, dane[i].c1, dane[i].c2, dane[i].c3);
                }
                else if (tryb == 2 || tryb == 5) { // YCbCr
                    setYCbCr(targetX, y, (float)dane[i].c1, (float)dane[i].c2, (float)dane[i].c3);
                }
            }
        }
    }

    delete[] dane;
    // Ważne: Odświeżenie okna, żeby zobaczyć efekt
    SDL_UpdateWindowSurface(window);
    cout << "[SUKCES] Obraz wyswietlony po prawej stronie." << endl;
}
bool czyPlikIstnieje(string nazwa) {
    ifstream f(nazwa.c_str());
    return f.good();
}

void AppMenu() {
    while(true) {
        string plikWe, plikWy;

        cout << "\n================== MENU P11 ==================" << endl;
        cout << "Podaj plik wejsciowy (lub 'exit'): ";
        if (!(cin >> plikWe)) break;
        if (plikWe == "exit") return;

        if (!czyPlikIstnieje(plikWe)) {
            cout << "[BLAD] Plik nie istnieje!" << endl;
            continue;
        }

        cout << "Podaj plik wyjsciowy: "; cin >> plikWy;

        string ext = "";
        if(plikWe.find_last_of(".") != string::npos)
            ext = plikWe.substr(plikWe.find_last_of(".") + 1);

        if (ext == "dg24" || ext == "DG24") {
            wczytajDG24(plikWe);

            // Zapis do BMP (prawa strona)
            SDL_Surface* screen = SDL_GetWindowSurface(window);

            Uint32 rmask, gmask, bmask, amask;
            #if SDL_BYTEORDER == SDL_BIG_ENDIAN
                rmask = 0xff000000; gmask = 0x00ff0000; bmask = 0x0000ff00; amask = 0x000000ff;
            #else
                rmask = 0x000000ff; gmask = 0x0000ff00; bmask = 0x00ff0000; amask = 0xff000000;
            #endif

            SDL_Surface* out = SDL_CreateRGBSurface(0, hwidth, hheight, 32, rmask, gmask, bmask, amask);
            SDL_Rect src = {hwidth, 0, hwidth, hheight};
            SDL_Rect dst = {0, 0, hwidth, hheight};

            SDL_BlitSurface(screen, &src, out, &dst);
            SDL_SaveBMP(out, plikWy.c_str());
            SDL_FreeSurface(out);
            cout << "[SUKCES] Zapisano BMP." << endl;
        }
        else if (ext == "bmp" || ext == "BMP") {
            ladujBMP(plikWe.c_str(), 0, 0);

            int wybor;
            uint8_t tryb = 0, pred = 0, komp = 0;

            cout << "Tryb barwny (1: 16-bit | 2: 24-bit): "; cin >> wybor;

            if (wybor == 1) { // 16-BIT
                cout << "Dithering? (1: Tak | 0: Nie): "; cin >> wybor;
                tryb = (wybor == 1) ? 3 : 0;
                cout << "Predykcja? (1: Tak | 0: Nie): "; cin >> wybor;
                pred = (wybor == 1) ? 1 : 0;
                cout << "Kompresja RLE? (1: Tak | 0: Nie): "; cin >> wybor;
                komp = (wybor == 1) ? 1 : 0;
            }
            else { // 24-BIT
                cout << "Model barwny (1: RGB | 0: YCbCr): "; cin >> wybor;
                int model = wybor; // 1=RGB, 0=YCbCr

                cout << "Skala szarosci? (1: Tak | 0: Nie): "; cin >> wybor;
                int isGray = wybor;

                if (model == 1) { // RGB
                    tryb = (isGray == 1) ? 4 : 1; // 4=Szary RGB, 1=RGB

                    cout << "Kompresja RLE? (1: Tak | 0: Nie): "; cin >> wybor;
                    komp = (wybor == 1) ? 1 : 0;

                    if (komp <= 1) {
                        cout << "Predykcja? (1: Tak | 0: Nie): "; cin >> wybor;
                        pred = (wybor == 1) ? 1 : 0;
                    }
                }
                else { // YCbCr
                    tryb = (isGray == 1) ? 5 : 2; // 5=Szary YCbCr, 2=YCbCr

                    cout << "Kompresja (2: Stratna | 1: RLE | 0: Brak): "; cin >> wybor;
                    komp = (uint8_t)wybor;

                    if (komp <= 1) {
                        cout << "Predykcja? (1: Tak | 0: Nie): "; cin >> wybor;
                        pred = (wybor == 1) ? 1 : 0;
                    } else {
                        pred = 0;
                    }
                }
            }

            zapiszDG24(plikWy, tryb, pred, komp);
        }
    }
}

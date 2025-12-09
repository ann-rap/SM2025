// podstawowe funkcje
#include "SM2025-Funkcje.h"
#include "SM2025-Zmienne.h"
#include "SM2025-Paleta.h"
#include "SM2025-MedianCut.h"
#include "SM2025-Pliki.h"
#include <iomanip>
const int pixel_count = hheight*hwidth;

// Globalne zmienne do przechowywania wyników
WynikStruct wynik1, wynik2, wynik3, wynik4, wynik5;
void Funkcja1() {//kompresja i dekompresja z tablicy
    //P6
    Kolor* colors = new Kolor[wysokosc/2 * szerokosc/2];
    for(int y = 0; y < wysokosc/2; y++) {
        for(int x = 0; x < szerokosc/2; x++) {
            SDL_Color nk = getPixel(x, y);
            colors[y * (szerokosc/2) + x].c1 = nk.r;
            colors[y * (szerokosc/2) + x].c2 = nk.g;
            colors[y * (szerokosc/2) + x].c3 = nk.b;
        }
    }

    ByteRunColors komp = kompresjaObrazu(colors, (wysokosc/2) * (szerokosc/2));
    Kolor* dekomp = dekompresjObrazu(&komp);

    for(int y = 0; y < wysokosc/2; y++) {
        for(int x = 0; x < szerokosc/2; x++) {
            Kolor kolor = dekomp[y * (szerokosc/2) + x];
            setPixel(x + hwidth, y, kolor.c1, kolor.c2, kolor.c3);
        }
    }





    SDL_UpdateWindowSurface (window);
}
////////Kompresja
void Funkcja2() {//zapis do pliku
    Kolor* colors = new Kolor[wysokosc/2 * szerokosc/2];
    for(int y = 0; y < wysokosc/2; y++) {
        for(int x = 0; x < szerokosc/2; x++) {
            SDL_Color nk = getPixel(x, y);
            colors[y * (szerokosc/2) + x].c1 = nk.r;
            colors[y * (szerokosc/2) + x].c2 = nk.g;
            colors[y * (szerokosc/2) + x].c3 = nk.b;
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
    Kolor* dekomp = dekompresjObrazu(&komp);

    for(int y = 0; y < wysokosc/2; y++) {
        for(int x = 0; x < szerokosc/2; x++) {
            Kolor kolor = dekomp[y * (szerokosc/2) + x];
            setPixel(x + hwidth, y, kolor.c1, kolor.c2, kolor.c3);
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
    Kolor* dekomp = dekompresjObrazu(loaded);

    for(int y = 0; y < wysokosc/2; y++) {
        for(int x = 0; x < szerokosc/2; x++) {
            Kolor kolor = dekomp[y * (szerokosc/2) + x];
            setPixel(x + hwidth, y+hheight, kolor.c1, kolor.c2, kolor.c3);
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
    //P4
    subsample420_HSL(szerokosc/2, wysokosc/2);

    SDL_UpdateWindowSurface(window);
}
void Funkcja6() {
   zaktualizujTabliceBayera4();  // Inicjalizacja tablicy Bayera

    for(int y = 0; y < wysokosc/2; y++){
        for(int x = 0; x < szerokosc/2; x++){
            Uint16 kolor = getRGB555D_(x, y);
            setRGB555(x, y + wysokosc/2, kolor);
        }
    }
    SDL_UpdateWindowSurface(window);
}

void Funkcja7() {
 zaktualizujTabliceBayera4();  // Inicjalizacja tablicy Bayera

    for(int y = 0; y < wysokosc/2; y++){
        for(int x = 0; x < szerokosc/2; x++){
            Uint16 nowyKolor = getRGB565D_(x, y);
            setRGB565(x + szerokosc/2, y + wysokosc/2, nowyKolor);
        }
    }
    SDL_UpdateWindowSurface(window);


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

    // Czyszczenie ekranu
    czyscEkran(0, 0, 0);

    SDL_UpdateWindowSurface(window);
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

//Sprawdzenie wartosci jednego wiersza obrazku do zadania z RLE, wywołanie przez "w"
void wypiszWiersz(int y) {
    if (y < 0 || y >= hheight) {
        cout << "Nieprawidlowy numer wiersza! (0-" << (hheight-1) << ")" << endl;
        return;
    }

    cout << "\n=== WIERSZ " << y << " ===" << endl;

    // Wiersz R (czerwony)
    cout << "R: ";
    for(int x = 0; x < hwidth; x++) {
        SDL_Color pixel = getPixel(x, y);
        cout << (int)pixel.r << " ";
    }
    cout << endl;

    // Wiersz G (zielony)
    cout << "G: ";
    for(int x = 0; x < hwidth; x++) {
        SDL_Color pixel = getPixel(x, y);
        cout << (int)pixel.g << " ";
    }
    cout << endl;

    // Wiersz B (niebieski)
    cout << "B: ";
    for(int x = 0; x < hwidth; x++) {
        SDL_Color pixel = getPixel(x, y);
        cout << (int)pixel.b << " ";
    }
    cout << endl << endl;

    int* test = new int[hwidth];
for(int x = 0; x < hwidth; x++) {
    SDL_Color pixel = getPixel(x, y);
    test[x] = pixel.r;
}
RLE* komp = RLEKompresja(test, hwidth);
cout << "Oryginalny rozmiar: " << hwidth << endl;
cout << "Po kompresji RLE: " << komp->len << endl;
cout << "Zmiana: " << (komp->len - hwidth) << endl;
delete komp;
delete[] test;
}


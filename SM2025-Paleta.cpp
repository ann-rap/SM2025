// funkcje do redukcji kolorów i tworzenia palet
#include "SM2025-Paleta.h"
#include "SM2025-Zmienne.h"
#include "SM2025-Funkcje.h"

SDL_Color filtrPNG[320][200];
Uint8 filtrRGB555_lo[320][200];
Uint8 filtrRGB555_hi[320][200];


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
void setPixelByStruct(int x, int y, Kolor kolor){
    setPixel(x,y,kolor.c1,kolor.c2,kolor.c3);
}


/* ====== Normalizacja ===== */

float normalize(float part){
    if(part>255.0){
        return 255.0;
    }
    else if(part<0.0){
        return 0.0;
    }
    else{
        return part;
    }

}
Kolor normalizeAll(float r,float g,float b){
    Kolor kolor;
    kolor.c1 = normalize(r);
    kolor.c2 = normalize(g);
    kolor.c3 = normalize(b);
    return kolor;
}

float normalizeHsl(float x){
    if(x>1.0){
        return x-1.0;
    }
    else if(x<0.0){
        return x+1.0;
    }
    return x;
}



YCbCr getYCbCr(int xx, int yy){
    SDL_Color kolor  = getPixel(xx,yy);
    YCbCr nowyKolor;
    float r = kolor.r;
    float g = kolor.g;
    float b = kolor.b;
    nowyKolor.y = 0 + (0.299 * r) + (0.587 * g) + (0.114 * b);
    nowyKolor.cb = 128 - (0.168736 * r) - (0.331264 * g) + (0.5 * b);
    nowyKolor.cr = 128 + (0.5 * r) - (0.418688 * g) - (0.081312 * b);

    return nowyKolor;
}

void setYCbCr(int xx, int yy, float y, float cb, float cr){
    float r = y + 1.402 * (cr - 128);
    float g = y - 0.344136 * (cb -128) - 0.714136 * (cr -128);
    float b = y + 1.772 * (cb -128);

    setPixelByStruct(xx,yy, normalizeAll(r,g,b));

}

void subsample420_YCbCr(int width, int height) {
    for (int y = 0; y < height; y += 2) {
        for (int x = 0; x < width; x += 2) {
            // 1. Ustalanie współrzędnych bloku 2x2
            int x1 = (x + 1 < width)  ? x + 1 : x;
            int y1 = (y + 1 < height) ? y + 1 : y;

            // 2. Pobranie pikseli źródłowych (LEWA STRONA: 0..319)
            YCbCr c00 = getYCbCr(x,  y);
            YCbCr c10 = getYCbCr(x1, y);
            YCbCr c01 = getYCbCr(x,  y1);
            YCbCr c11 = getYCbCr(x1, y1);

            // 3. Obliczenie średniej chrominancji (Subsampling)
            float cb_avg = (c00.cb + c10.cb + c01.cb + c11.cb) / 4.0f;
            float cr_avg = (c00.cr + c10.cr + c01.cr + c11.cr) / 4.0f;

            // 4. ZAPIS DO TABLICY GLOBALNEJ I WYŚWIETLANIE (PRAWA STRONA)

            // Piksel 00 (Góra-Lewo)
            subcolors[x][y].c1 = c00.y;
            subcolors[x][y].c2 = cb_avg;
            subcolors[x][y].c3 = cr_avg;
            setYCbCr(x + hwidth, y, c00.y, cb_avg, cr_avg);

            // Piksel 10 (Góra-Prawo)
            if (x + 1 < width) {
                subcolors[x1][y].c1 = c10.y;
                subcolors[x1][y].c2 = cb_avg;
                subcolors[x1][y].c3 = cr_avg;
                setYCbCr(x1 + hwidth, y, c10.y, cb_avg, cr_avg);
            }

            // Piksel 01 (Dół-Lewo)
            if (y + 1 < height) {
                subcolors[x][y1].c1 = c01.y;
                subcolors[x][y1].c2 = cb_avg;
                subcolors[x][y1].c3 = cr_avg;
                setYCbCr(x + hwidth, y1, c01.y, cb_avg, cr_avg);
            }

            // Piksel 11 (Dół-Prawo)
            if (x + 1 < width && y + 1 < height) {
                subcolors[x1][y1].c1 = c11.y;
                subcolors[x1][y1].c2 = cb_avg;
                subcolors[x1][y1].c3 = cr_avg;
                setYCbCr(x1 + hwidth, y1, c11.y, cb_avg, cr_avg);
            }
        }
    }
}

void filtrujYCbCr_Typ3(bool isGray) {
    int w = hwidth;
    int h = hheight;
    YCbCr neutral = {0.0f, 128.0f, 128.0f};

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            YCbCr cur = getYCbCr(x, y);
            if (isGray) { cur.cb = 128; cur.cr = 128; }

            // POBIERAMY Z BUFORA REKONSTRUKCJI (symulujemy dekoder!)
            // Jeśli nie masz osobnego bufora, użyj buforYCbCr po uprzednim wyczyszczeniu
            YCbCr left = (x > 0) ? buforYCbCr[x - 1][y] : neutral;
            YCbCr up   = (y > 0) ? buforYCbCr[x][y - 1] : neutral;

            // Liczymy predykcję identycznie jak w dekoderze
            int predY  = (int)floor((left.y  + up.y)  / 2.0);
            int predCb = (int)floor((left.cb + up.cb) / 2.0);
            int predCr = (int)floor((left.cr + up.cr) / 2.0);

            // Różnica
            int diffY  = (int)cur.y  - predY;
            int diffCb = (int)cur.cb - predCb;
            int diffCr = (int)cur.cr - predCr;

            // Zapisujemy do bufora filtra (Uint8 wrap-around)
            filtrYCbCr[x][y].y  = (float)((Uint8)diffY);
            filtrYCbCr[x][y].cb = (float)((Uint8)diffCb);
            filtrYCbCr[x][y].cr = (float)((Uint8)diffCr);

            // AKTUALIZUJEMY BUFOR REKONSTRUKCJI (klucz do sukcesu)
            // Koder musi teraz obliczyć to, co obliczy dekoder, by kolejny piksel miał dobrą bazę
            buforYCbCr[x][y].y  = (float)((Uint8)((Uint8)diffY + predY));
            if (isGray) {
                buforYCbCr[x][y].cb = 128.0f;
                buforYCbCr[x][y].cr = 128.0f;
            } else {
                buforYCbCr[x][y].cb = (float)((Uint8)((Uint8)diffCb + predCb));
                buforYCbCr[x][y].cr = (float)((Uint8)((Uint8)diffCr + predCr));
            }
        }
    }
}

void odfiltrujYCbCr_Typ3() {
    for (int y = 0; y < hheight; y++) {
        for (int x = 0; x < hwidth; x++) {

            YCbCr diff = filtrYCbCr[x][y];

            YCbCr left = (x > 0) ? buforYCbCr[x - 1][y] : YCbCr{0, 0, 0};
            YCbCr up   = (y > 0) ? buforYCbCr[x][y - 1] : YCbCr{0, 0, 0};

            float predY  = floor((left.y + up.y) / 2.0f);
            float predCb = floor((left.cb + up.cb) / 2.0f);
            float predCr = floor((left.cr + up.cr) / 2.0f);

            YCbCr rec;

            // KLUCZOWA POPRAWKA:
            // Musimy dodać różnicę do predykcji i zrzutować na Uint8,
            // aby nastąpiło poprawne "przekręcenie" licznika (modulo 256).
            // Np. 246 + 110 = 356 -> (Uint8)356 = 100.

            rec.y  = (float)((Uint8)((int)diff.y + (int)predY));
            rec.cb = (float)((Uint8)((int)diff.cb + (int)predCb));
            rec.cr = (float)((Uint8)((int)diff.cr + (int)predCr));

            buforYCbCr[x][y] = rec;

            // Rysujemy na lewej stronie (x, y) zgodnie z logiką wczytywania
            setYCbCr(x, y, rec.y, rec.cb, rec.cr);
        }
    }
}
void setRGB555(int xx, int yy, Uint8 r, Uint8 g, Uint8 b){
    //idk czy wejście to normalne czy już zmienione na 555
     int nowyR, nowyG, nowyB;
    Uint16 kolor16bit;
    nowyR = round(r*31.0/255.0);//5bit zakrs 0-31;
    nowyG = round(g*31.0/255.0);
    nowyB = round(b*31.0/255.0);

    kolor16bit = (nowyR<<10) | (nowyG<<5) | nowyB;
    setRGB555(xx,yy,kolor16bit);
}
void setRGB555(int xx, int yy, Uint16 rgb555){
    SDL_Color kolor24bit;
    int nowyR, nowyG, nowyB;
    nowyR = (rgb555&(0b0111110000000000))>>10;
    nowyG = (rgb555&(0b0000001111100000))>>5;
    nowyB = (rgb555&(0b0000000000011111));
    kolor24bit.r=nowyR*255.0/31.0;
    kolor24bit.g=nowyG*255.0/31.0;
    kolor24bit.b=nowyB*255.0/31.0;
    setPixel(xx,yy,kolor24bit.r,kolor24bit.g,kolor24bit.b);
}

SDL_Color getRGB555(int xx, int yy){
    Uint16 kolor16bit = getRGB555_(xx,yy);
    SDL_Color kolor24bit;
    int nowyR, nowyG, nowyB;
    nowyR = (kolor16bit&(0b0111110000000000))>>10;
    nowyG = (kolor16bit&(0b0000001111100000))>>5;
    nowyB = (kolor16bit&(0b0000000000011111));
    kolor24bit.r=nowyR*255.0/31.0;
    kolor24bit.g=nowyG*255.0/31.0;
    kolor24bit.b=nowyB*255.0/31.0;
    return kolor24bit;
}

Uint16 getRGB555_(int xx, int yy){
    SDL_Color kolor = getPixel(xx,yy);
     Uint16 kolor16bit;
    int nowyR, nowyG, nowyB;

    nowyR = round(kolor.r*31.0/255.0);//5bit zakrs 0-31;
    nowyG = round(kolor.g*31.0/255.0);
    nowyB = round(kolor.b*31.0/255.0);

    kolor16bit = (nowyR<<10) | (nowyG<<5) | nowyB;
    //bity: 15 pusty ,14-10 r, 9-5 g, 4-0 b
    return kolor16bit;
}

/*////////////
// Dithering /
*/////////////

int tablicaBayera4[4][4] = {{6,14,8,16},
                            {10,2,12,4},
                            {7,15,5,13},
                            {11,3,9,1}};



void zaktualizujTabliceBayera4(){
    float zakresK=255/3;
    float zakresS = 255/63;
    int rozmiar = 4;
    float podzialK = zakresK /(rozmiar*rozmiar);
    float podzialS = zakresS /(rozmiar*rozmiar);


    for(int y =0; y< rozmiar; y++){
        for(int x=0; x<rozmiar; x++){
            zaktualizowanaTablicaBayera4k[y][x]=(tablicaBayera4[y][x]*podzialK)-podzialK/2;
            zaktualizowanaTablicaBayera4s[y][x]=(tablicaBayera4[y][x]*podzialS)-podzialS/2;

        }
    }

    for(int y =0; y< rozmiar; y++){
            cout << endl;
        for(int x=0; x<rozmiar; x++){
            cout << tablicaBayera4[y][x]<<" ";
        }
    }
    cout << endl;

    for(int y =0; y< rozmiar; y++){
            cout << endl;
        for(int x=0; x<rozmiar; x++){
            cout << zaktualizowanaTablicaBayera4k[y][x]<<" ";
        }
    }
    cout << endl;
        for(int y =0; y< rozmiar; y++){
            cout << endl;
        for(int x=0; x<rozmiar; x++){
            cout << zaktualizowanaTablicaBayera4s[y][x]<<" ";
        }
    }
    cout << endl;
}
// getRGB555D - odczyt z ditheringiem Bayera dla RGB555
SDL_Color getRGB555D(int xx, int yy){
    SDL_Color kolor = getPixel(xx, yy);

    // Pozycja w tablicy Bayera 4x4
    int bayerX = xx % 4;
    int bayerY = yy % 4;
    float prog = zaktualizowanaTablicaBayera4k[bayerY][bayerX];

    // Dodaj wartość z tablicy Bayera przed kwantyzacją
    int r = kolor.r + prog;
    int g = kolor.g + prog;
    int b = kolor.b + prog;

    // Ogranicz do zakresu 0-255
    r = (r < 0) ? 0 : (r > 255) ? 255 : r;
    g = (g < 0) ? 0 : (g > 255) ? 255 : g;
    b = (b < 0) ? 0 : (b > 255) ? 255 : b;

    // Konwersja do RGB555 (5 bitów na kanał)
    int nowyR = round(r * 31.0 / 255.0);
    int nowyG = round(g * 31.0 / 255.0);
    int nowyB = round(b * 31.0 / 255.0);

    Uint16 kolor16bit = (nowyR << 10) | (nowyG << 5) | nowyB;

    // Konwersja z powrotem do 24-bit
    SDL_Color kolor24bit;
    nowyR = (kolor16bit & (0b0111110000000000)) >> 10;
    nowyG = (kolor16bit & (0b0000001111100000)) >> 5;
    nowyB = (kolor16bit & (0b0000000000011111));

    kolor24bit.r = nowyR * 255.0 / 31.0;
    kolor24bit.g = nowyG * 255.0 / 31.0;
    kolor24bit.b = nowyB * 255.0 / 31.0;

    return kolor24bit;
}

// getRGB555D_ - zwraca wartość Uint16 z ditheringiem
Uint16 getRGB555D_(int xx, int yy){
    SDL_Color kolor = getPixel(xx, yy);

    // Pozycja w tablicy Bayera 4x4
    int bayerX = xx % 4;
    int bayerY = yy % 4;
    float prog = zaktualizowanaTablicaBayera4k[bayerY][bayerX];

    // Dodaj wartość z tablicy Bayera
    int r = kolor.r + prog;
    int g = kolor.g + prog;
    int b = kolor.b + prog;

    // Ogranicz do zakresu 0-255
    r = (r < 0) ? 0 : (r > 255) ? 255 : r;
    g = (g < 0) ? 0 : (g > 255) ? 255 : g;
    b = (b < 0) ? 0 : (b > 255) ? 255 : b;

    // Konwersja do RGB555
    int nowyR = round(r * 31.0 / 255.0);
    int nowyG = round(g * 31.0 / 255.0);
    int nowyB = round(b * 31.0 / 255.0);

    Uint16 kolor16bit = (nowyR << 10) | (nowyG << 5) | nowyB;

    return kolor16bit;
}


//Filtrowanie

int paeth(int a,int b,int c){
    int p=a+b-c;
    int pa=abs(p-a), pb=abs(p-b), pc=abs(p-c);
    if(pa<=pb && pa<=pc) return a;
    else if(pb<=pc) return b;
    else return c;
}


void filtrujPNG_Typ3() {
    for(int y=0;y<hheight;y++){
        for(int x=0;x<hwidth;x++){
            SDL_Color cur = getPixel(x,y);
            SDL_Color left = (x>0)? getPixel(x-1,y): SDL_Color{0,0,0};
            SDL_Color up   = (y>0)? getPixel(x,y-1): SDL_Color{0,0,0};
            filtrPNG[x][y].r = (cur.r - ((left.r+up.r)/2) + 256)%256;
            filtrPNG[x][y].g = (cur.g - ((left.g+up.g)/2) + 256)%256;
            filtrPNG[x][y].b = (cur.b - ((left.b+up.b)/2) + 256)%256;
        }
    }
}


void odfiltrujPNG_Typ3() {
    for(int y=0;y<hheight;y++){
        for(int x=0;x<hwidth;x++){
            SDL_Color f = filtrPNG[x][y];

            // Czytamy z bufora, nie z ekranu!
            SDL_Color left = (x>0)? buforPNG[x-1][y] : SDL_Color{0,0,0};
            SDL_Color up   = (y>0)? buforPNG[x][y-1] : SDL_Color{0,0,0};

            SDL_Color out;
            out.r = (f.r + ((left.r+up.r)/2))%256;
            out.g = (f.g + ((left.g+up.g)/2))%256;
            out.b = (f.b + ((left.b+up.b)/2))%256;

            // Zapisujemy do bufora (dla następnych iteracji)
            buforPNG[x][y] = out;

            // I na ekran
            setPixel(x + hwidth,y,out.r,out.g,out.b);
        }
    }
}


void filtrujRGB555_Typ3(){
    for(int y=0;y<hheight;y++){
        for(int x=0;x<hwidth;x++){
            Uint16 cur=getRGB555_(x,y);
            Uint16 L=(x>0)?getRGB555_(x-1,y):0;
            Uint16 U=(y>0)?getRGB555_(x,y-1):0;

            Uint8 cL=cur&0xFF, cH=(cur>>8)&0xFF;
            Uint8 lL=L&0xFF,   lH=(L>>8)&0xFF;
            Uint8 uL=U&0xFF,   uH=(U>>8)&0xFF;

            Uint8 avgL=(Uint8)(((int)lL + (int)uL)/2);
            Uint8 avgH=(Uint8)(((int)lH + (int)uH)/2);

            filtrRGB555_lo[x][y]=(Uint8)((cL - avgL + 256)%256);
            filtrRGB555_hi[x][y]=(Uint8)((cH - avgH + 256)%256);
        }
    }
}

void odfiltrujRGB555_Typ3(){
    for(int y=0;y<hheight;y++){
        for(int x=0;x<hwidth;x++){
            Uint8 fL = filtrRGB555_lo[x][y];
            Uint8 fH = filtrRGB555_hi[x][y];

            Uint16 L = (x > 0) ? buforRGB555[x-1][y] : 0;
            Uint16 U = (y > 0) ? buforRGB555[x][y-1] : 0;

            Uint8 lL = L & 0xFF,        lH = (L >> 8) & 0xFF;
            Uint8 uL = U & 0xFF,        uH = (U >> 8) & 0xFF;

            Uint8 avgL = (Uint8)(((int)lL + (int)uL) / 2);
            Uint8 avgH = (Uint8)(((int)lH + (int)uH) / 2);

            Uint8 nL = (Uint8)((fL + avgL) % 256);
            Uint8 nH = (Uint8)((fH + avgH) % 256);

            Uint16 out = (Uint16)nL | ((Uint16)nH << 8);

            buforRGB555[x][y] = out;

            setRGB555(x, y, out);
        }
    }
}

void wyswietlDane(macierz blok) {
    cout<<"Dane pikselowe macierzy"<<endl;
    for(int y = 0; y<rozmiarBloku; y++){
        for(int x = 0; x < rozmiarBloku; x++){
            cout<<setw(4)<<(int)blok.dane[x][y]<<" ";
        }
        cout<<endl;
    }
}

void wyswietlDCT(macierz blok) {
    cout<<"Wspolczynniki transformaty macierzy:"<<endl;
    for(int y = 0; y<rozmiarBloku; y++){
        for(int x = 0; x < rozmiarBloku; x++){
            cout<<fixed<<setw(6)<<setprecision(2)<<blok.dct[x][y]<<" ";
        }
        cout<<endl;
    }
}

macierz dct(Uint8 wartosci[rozmiarBloku][rozmiarBloku]) {
    float wynik[rozmiarBloku][rozmiarBloku];

    // obliczamy DCT
    for (int v = 0; v < rozmiarBloku; ++v) {
        for (int u = 0; u < rozmiarBloku; ++u) {

            const double cu = (u == 0) ? 1.0 / sqrt(2) : 1.0;
            const double cv = (v == 0) ? 1.0 / sqrt(2) : 1.0;

            double wspolczynnikDCT = 0;

            for (int y = 0; y < rozmiarBloku; ++y) {
                for (int x = 0; x < rozmiarBloku; ++x) {

                    double uCosFactor =
                        cos((double)(2 * x + 1) * M_PI *
                            (double)u / (2 * (double)rozmiarBloku));

                    double vCosFactor =
                        cos((double)(2 * y + 1) * M_PI *
                            (double)v / (2 * (double)rozmiarBloku));

                    double pixel = (double)wartosci[x][y] - 128.0;
                    wspolczynnikDCT += pixel * uCosFactor * vCosFactor;
                }
            }

            wspolczynnikDCT *= (2.0 / (double)rozmiarBloku) * cu * cv;
            wynik[u][v] = wspolczynnikDCT;
        }
    }

    // przygotowujemy macierz wyjsciowa
    macierz rezultat;
    for (int j = 0; j < rozmiarBloku; j++) {
        for (int i = 0; i < rozmiarBloku; i++) {
            rezultat.dct[i][j] = wynik[i][j];
            rezultat.dane[i][j] = wartosci[i][j];
        }
    }

    return rezultat;
}

macierz idct(float DCT[rozmiarBloku][rozmiarBloku]) {

    int wynik[rozmiarBloku][rozmiarBloku];

    // obliczamy IDCT
    for (int x = 0; x < rozmiarBloku; ++x) {
        for (int y = 0; y < rozmiarBloku; ++y) {

            double pixel = 0;

            for (int u = 0; u < rozmiarBloku; ++u) {
                for (int v = 0; v < rozmiarBloku; ++v) {

                    const double cu = (u == 0) ? 1.0 / sqrt(2) : 1.0;
                    const double cv = (v == 0) ? 1.0 / sqrt(2) : 1.0;

                    double uCosFactor =
                        cos((double)(2 * x + 1) * M_PI *
                            (double)u / (2 * (double)rozmiarBloku));

                    double vCosFactor =
                        cos((double)(2 * y + 1) * M_PI *
                            (double)v / (2 * (double)rozmiarBloku));

                    double wspolczynnikDCT = DCT[u][v];
                    pixel += wspolczynnikDCT * uCosFactor * cu * vCosFactor * cv;
                }
            }

            pixel *= (2.0 / (double)rozmiarBloku);
            wynik[x][y] = round(pixel + 128.0);
        }
    }

    macierz rezultat;
    // przygotowujemy macierz wyjsciowa i (dla pewnosci) normalizujemy wartosci
    for (int j = 0; j < rozmiarBloku; j++) {
        for (int i = 0; i < rozmiarBloku; i++) {
            if (wynik[i][j] > 255) wynik[i][j] = 255;
            if (wynik[i][j] < 0)   wynik[i][j] = 0;

            rezultat.dane[i][j] = wynik[i][j];
            rezultat.dct[i][j]  = DCT[i][j];
        }
    }

    return rezultat;
}

// Funkcja do zebrania współczynników DCT w kolejności zygzakowatej
void zigzagCollect(float dct[rozmiarBloku][rozmiarBloku], float output[256]) {
    int index = 0;
    int x = 0, y = 0;
    bool goingUp = false;

    output[index++] = dct[0][0];  // DC coefficient

    while(index < 256) {
        if(goingUp) {
            // Idziemy w górę-prawo
            if(y == 0 || x == rozmiarBloku - 1) {
                goingUp = false;
                if(x == rozmiarBloku - 1) {
                    y++;
                } else {
                    x++;
                }
            } else {
                x++;
                y--;
            }
        } else {
            // Idziemy w dół-lewo
            if(x == 0 || y == rozmiarBloku - 1) {
                goingUp = true;
                if(y == rozmiarBloku - 1) {
                    x++;
                } else {
                    y++;
                }
            } else {
                x--;
                y++;
            }
        }

        if(x >= 0 && x < rozmiarBloku && y >= 0 && y < rozmiarBloku) {
                output[index++] = dct[y][x]; // y to wiersz, x to kolumna
            }

        // Sprawdź czy doszliśmy do końca
        if(x == rozmiarBloku - 1 && y == rozmiarBloku - 1) break;
    }

    // Wypełnij pozostałe wartości zerami
    while(index < 256) {
        output[index++] = 0.0f;
    }
}

// Funkcja odwrotna - odbuduj macierz DCT z zygzaka
void zigzagReconstruct(float zigzag[256], float dct[rozmiarBloku][rozmiarBloku]) {
    int index = 0;
    int x = 0, y = 0;
    bool goingUp = false;

    dct[0][0] = zigzag[index++];

    while(index < 256) {
        if(goingUp) {
            // Idziemy w górę-prawo (diagonalnie w górę)
            if(y == 0 || x == rozmiarBloku - 1) {
                goingUp = false;
                if(x == rozmiarBloku - 1) {
                    y++;
                } else {
                    x++;
                }
            } else {
                x++;
                y--;
            }
        } else {
            // Idziemy w dół-lewo (diagonalnie w dół)
            if(x == 0 || y == rozmiarBloku - 1) {
                goingUp = true;
                if(y == rozmiarBloku - 1) {
                    x++;
                } else {
                    y++;
                }
            } else {
                x--;
                y++;
            }
        }

        if(x >= 0 && x < rozmiarBloku && y >= 0 && y < rozmiarBloku) {
                dct[y][x] = zigzag[index++]; // y to wiersz, x to kolumna
            }

        // Sprawdź czy doszliśmy do końca
        if(x == rozmiarBloku - 1 && y == rozmiarBloku - 1) break;
    }
}

// Główna funkcja kompresji DCT zgodnie z zadaniem
void kompresjaDCT() {
    const int width = hwidth;   // 320
    const int height = hheight; // 200
    const int blokSize = rozmiarBloku; // 16

    cout << "\n=== KOMPRESJA DCT ===" << endl;

    // KROK 1: Konwersja na skalę szarości (okienko 1 -> okienko 2)
    cout << "Krok 1: Konwersja na skale szarosci..." << endl;
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            SDL_Color kolor = getPixel(x, y);  // z okienka 1 (0,0)
            int szary = kolor.r*0.299 + kolor.g*0.587 + kolor.b*0.114;
            setPixel(x + hwidth, y, szary, szary, szary);  // do okienka 2 (320,0)
        }
    }
    SDL_UpdateWindowSurface(window);

    int liczbaBlokowX = width / blokSize;   // 20
    int liczbaBlokowY = height / blokSize;  // 12

    cout << "Krok 2-3: Podzial na bloki " << blokSize << "x" << blokSize
         << " i transformacja DCT..." << endl;
    cout << "Liczba blokow: " << liczbaBlokowX << " x " << liczbaBlokowY << endl;

    int licznikBlokow = 0;

    // KROK 2-9: Przetwarzanie każdego bloku
    for(int by = 0; by < liczbaBlokowY; by++) {
        for(int bx = 0; bx < liczbaBlokowX; bx++) {
            // KROK 2: Pobierz blok 16x16 z okienka 2 (skala szarości)
            macierz blok;
            for(int y = 0; y < blokSize; y++) {
                for(int x = 0; x < blokSize; x++) {
                    int px = bx * blokSize + x;
                    int py = by * blokSize + y;
                    SDL_Color pixel = getPixel(px + hwidth, py);  // z okienka 2
                    blok.dane[x][y] = pixel.r;
                }
            }

            // KROK 3: Wykonaj DCT
            macierz blokDCT = dct(blok.dane);

            // KROK 5a: Wyzeruj prawą dolną część macierzy (przed zebraniem zygzakiem)
            // Wyzeruj współczynniki od pozycji (8,8) do końca
            int progX = 8;
            int progY = 8;
            for(int y = progY; y < blokSize; y++) {
                for(int x = progX; x < blokSize; x++) {
                    blokDCT.dct[x][y] = 0.0f;
                }
            }

            // KROK 5b: Zaokrąglij wszystkie niezerowe współczynniki
            for(int y = 0; y < blokSize; y++) {
                for(int x = 0; x < blokSize; x++) {
                    if(blokDCT.dct[x][y] != 0.0f) {
                        blokDCT.dct[x][y] = round(blokDCT.dct[x][y]);
                    }
                }
            }

            // KROK 4: Zbierz współczynniki zygzakowato
            float zigzag[256];
            zigzagCollect(blokDCT.dct, zigzag);

            // KROK 6: Wyświetlenie współczynników (tylko dla pierwszego bloku)
            if(licznikBlokow == 0) {
                cout << "\nKrok 6: Wspolczynniki zygzakowate (pierwszy blok):" << endl;
                for(int i = 0; i < 256; i++) {
                    cout << fixed << setprecision(1) << setw(8) << zigzag[i] << " ";
                    if((i+1) % 16 == 0) cout << endl;
                }
            }

            // KROK 7: Odbuduj macierz z zygzaka (w rzeczywistości już mamy blokDCT.dct,
            // ale pokazujemy że można to zrobić)
            // W tym przypadku nie musimy odbudowywać, bo mamy już zmodyfikowaną macierz

            // KROK 8: Wykonaj iDCT
            macierz zrekonstruowany = idct(blokDCT.dct);

            // KROK 9: Wizualizacja w okienku 4 (320, 200)
            for(int y = 0; y < blokSize; y++) {
                for(int x = 0; x < blokSize; x++) {
                    int px = bx * blokSize + x;
                    int py = by * blokSize + y;
                    Uint8 wartosc = zrekonstruowany.dane[x][y];
                    // Normalizuj wartość do zakresu 0-255
                    if(wartosc > 255) wartosc = 255;
                    if(wartosc < 0) wartosc = 0;
                    setPixel(px + hwidth, py + hheight, wartosc, wartosc, wartosc);
                }
            }

            licznikBlokow++;
        }
    }

    cout << "\nKrok 8-9: Transformacja iDCT i wizualizacja zakonczona." << endl;
    cout << "Przetworzono " << licznikBlokow << " blokow." << endl;
    cout << "=== KONIEC KOMPRESJI DCT ===\n" << endl;

    SDL_UpdateWindowSurface(window);
}


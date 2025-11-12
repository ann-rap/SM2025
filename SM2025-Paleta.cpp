// funkcje do redukcji kolorów i tworzenia palet
#include "SM2025-Paleta.h"
#include "SM2025-Zmienne.h"
#include "SM2025-Funkcje.h"

SDL_Color filtrPNG[320][200];
YUV filtrYUV[320][200];
Uint8 filtrRGB555_lo[320][200];
Uint8 filtrRGB555_hi[320][200];
Uint8 filtrRGB565_lo[320][200];
Uint8 filtrRGB565_hi[320][200];

YUV buforYUV[320][200];

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

void paletaWykryta6K(WynikStruct* wynik)
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


void paletaNarzucona6BIT(WynikStruct* wynik)
{
    Uint8 kolor6bit;
    SDL_Color kolor, nowyKolor;
    int index=0;

    czyscPalete6K();

    for (int y=0; y<wysokosc/2; y++){
        for (int x=0; x<szerokosc/2; x++){
            kolor = getPixel(x,y);

            kolor6bit = z24Kdo6K(kolor);
            nowyKolor = z6Kdo24K(kolor6bit);

            //na biezaco zapisujemy indeksy do palety i wypelniamy zmienna paleta6
            int indeksKoloru=sprawdzKolor6K(nowyKolor);
            wynik->indeksy[index]=indeksKoloru;
            index++;

            setPixel(x + szerokosc/2, y, nowyKolor.r, nowyKolor.g, nowyKolor.b);
        }
    }
    //skopiowanie paleta6 do struktury wyniku
     for (int i = 0; i < 64; i++) {
        wynik->paleta[i] = paleta6[i];
    }


}

void paletaNarzuconaSzary6BIT(WynikStruct* wynik)
{
    Uint8  szary6bit;
    SDL_Color szary, kolor, nowySzary;
    int index = 0;

    czyscPalete6K();

    for (int y=0; y<wysokosc/2; y++){
        for (int x=szerokosc/2; x<szerokosc; x++){
            kolor = getPixel(x,y);
            szary6bit = z24Kdo6Kszary(kolor);
            nowySzary = z6Kdo24Kszary(szary6bit);

             //na biezaco zapisujemy indeksy do palety i wypelniamy zmienna paleta6
            int indeksKoloru=sprawdzKolor6K(nowySzary);
            wynik->indeksy[index]=indeksKoloru;
            index++;

            setPixel(x, y, nowySzary.r, nowySzary.g, nowySzary.b);
        }
    }
    //skopiowanie paleta6 do struktury wyniku
    for (int i = 0; i < 64; i++) {
        wynik->paleta[i] = paleta6[i];
    }

}

/*===== Set z strukturą =====*/

void setPixelByStruct(int x, int y, Kolor kolor){
    setPixel(x,y,kolor.r,kolor.g,kolor.b);
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
    kolor.r = normalize(r);
    kolor.g = normalize(g);
    kolor.b = normalize(b);
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

/* ====== YUV ===== */

YUV getYUV(int xx, int yy){
    SDL_Color kolor  = getPixel(xx,yy);
    YUV newColor;
    float r = kolor.r;
    float g = kolor.g;
    float b = kolor.b;
    newColor.y = (0.299 * r) + (0.587 * g) + (0.114 * b);
    newColor.u = (-0.14713 * r) + (-0.28886 * g) + (0.436 * b);
    newColor.v = (0.615 * r) + (-0.51499 * g) + (-0.10001 * b);

    return newColor;
}

void setYUV(int xx, int yy, float y, float u, float v){
    float r = y + (0 * u) + (1.13983 * v);
    float g = y + (-0.39465 * u) + (-0.58060 * v);
    float b = y + (2.03211 * u) + (0 * v);

    setPixelByStruct(xx,yy, normalizeAll(r,g,b));

}

//P4
void subsample420_YUV(int width, int height){
    for (int y = 0; y < height; y += 2) {
        for (int x = 0; x < width; x += 2) {
            int x1 = (x + 1 < width)  ? x + 1 : x;
            int y1 = (y + 1 < height) ? y + 1 : y;

            YUV c00 = getYUV(x,  y );
            YUV c10 = getYUV(x1, y );
            YUV c01 = getYUV(x,  y1);
            YUV c11 = getYUV(x1, y1);

            //Chrominancji U i V
            float u_avg = (c00.u + c10.u + c01.u + c11.u) / 4.0f;
            float v_avg = (c00.v + c10.v + c01.v + c11.v) / 4.0f;

            setYUV(x+szerokosc/2,  y,  c00.y, u_avg, v_avg);
            if (x + 1 < width)
                setYUV(x1+szerokosc/2, y,  c10.y, u_avg, v_avg);
            if (y + 1 < height)
                setYUV(x+szerokosc/2,  y1, c01.y, u_avg, v_avg);
            if (x + 1 < width && y + 1 < height)
                setYUV(x1+szerokosc/2, y1, c11.y, u_avg, v_avg);
        }
    }
}
/* ====== YIQ ===== */

YIQ getYIQ(int xx,int yy){
    SDL_Color kolor  = getPixel(xx,yy);
    YIQ newColor;
    float r = kolor.r;
    float g = kolor.g;
    float b = kolor.b;
    newColor.y = (0.299 * r) + (0.587 *g) + (0.114*b);
    newColor.i = (0.5959 * r) + (-0.2746 *g) + (-0.3213*b);
    newColor.q = (0.2115 * r) + (-0.5227 *g) + (0.3112*b);


    return newColor;

}

void setYIQ(int xx, int yy, float y, float i, float q){
    float r = y + (0.956 * i) + (0.619 * q);
    float g = y + (-0.272 * i) + (-0.647 * q);
    float b = y + (-1.106 * i) + (1.703 * q);

 setPixelByStruct(xx,yy, normalizeAll(r,g,b));

 }

 //P4
 void subsample420_YIQ(int width, int height){
    for (int y = 0; y < height; y += 2) {
        for (int x = 0; x < width; x += 2) {
            int x1 = (x + 1 < width)  ? x + 1 : x;
            int y1 = (y + 1 < height) ? y + 1 : y;

            YIQ c00 = getYIQ(x,  y );
            YIQ c10 = getYIQ(x1, y );
            YIQ c01 = getYIQ(x,  y1);
            YIQ c11 = getYIQ(x1, y1);

            //Chrominancji I i Q
            float i_avg = (c00.i + c10.i + c01.i + c11.i) / 4.0f;
            float q_avg = (c00.q + c10.q + c01.q + c11.q) / 4.0f;

            setYIQ(x,  y+wysokosc/2,  c00.y, i_avg, q_avg);
            if (x + 1 < width)
                setYIQ(x1, y+wysokosc/2,  c10.y, i_avg, q_avg);
            if (y + 1 < height)
                setYIQ(x,  y1+wysokosc/2, c01.y, i_avg, q_avg);
            if (x + 1 < width && y + 1 < height)
                setYIQ(x1, y1+wysokosc/2, c11.y, i_avg, q_avg);
        }
    }
}

 /* ====== YCbCr ===== */

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

//P4
void subsample420_YCbCr(int width, int height){
    for (int y = 0; y < height; y += 2) {
        for (int x = 0; x < width; x += 2) {
            int x1 = (x + 1 < width)  ? x + 1 : x;
            int y1 = (y + 1 < height) ? y + 1 : y;

            YCbCr c00 = getYCbCr(x,  y );
            YCbCr c10 = getYCbCr(x1, y );
            YCbCr c01 = getYCbCr(x,  y1);
            YCbCr c11 = getYCbCr(x1, y1);

            //Chrominancji Cb i Cr
            float cb_avg = (c00.cb + c10.cb + c01.cb + c11.cb) / 4.0f;
            float cr_avg = (c00.cr + c10.cr + c01.cr + c11.cr) / 4.0f;

            setYCbCr(x+szerokosc/2,  y+wysokosc/2,  c00.y, cb_avg, cr_avg);
            if (x + 1 < width)
                setYCbCr(x1+szerokosc/2, y+wysokosc/2,  c10.y, cb_avg, cr_avg);
            if (y + 1 < height)
                setYCbCr(x+szerokosc/2,  y1+wysokosc/2, c01.y, cb_avg, cr_avg);
            if (x + 1 < width && y + 1 < height)
                setYCbCr(x1+szerokosc/2, y1+wysokosc/2, c11.y, cb_avg, cr_avg);
        }
    }
}

/* ===== HSL ===== */

HSL getHSL(int xx, int yy){
    SDL_Color base  = getPixel(xx,yy);

    float r = base.r/255.0;
    float g = base.g/255.0;
    float b = base.b/255.0;

    float maxRGB = max(max(r,g),b);
    float minRGB = min(min(r,g),b);
    float lum = (maxRGB + minRGB )/2;
    float sat;

    //cout<<maxRGB<<" , "<<minRGB<<endl;

    float hue = 0.0f;

    if (minRGB == maxRGB) {
        sat = 0.0;
        hue = 0.0;
    } else {
        if (lum <= 0.5) {
            sat = (maxRGB-minRGB)/ (maxRGB+minRGB);
        } else {
            sat = (maxRGB-minRGB)/ (2.0 - maxRGB - minRGB);
        }

        if (r == maxRGB) {
            hue = (g-b)/ (maxRGB-minRGB);
        } else if (g == maxRGB) {
            hue = 2.0 + (b-r)/(maxRGB-minRGB);
        } else {
            hue = 4.0 + (r-g)/(maxRGB-minRGB);
        }

        hue *= 60.0;
        if (hue < 0.0) hue += 360.0;
    }


     HSL hsl;
     hsl.h = hue;
     hsl.s = sat;
     hsl.l = lum;

     //cout<<hue<<" , "<<sat<<" , "<<lum<<endl;
     return hsl;

}

void setHSL(int xx, int yy, float h, float s, float l){
    float r,g,b, var1, var2,barwa, zmienaR,zmienaG,zmienaB;
    if(s==0.0){
        r = l*255;
        g=b=r;
        setPixel(xx,yy,r,g,b);
        return;
    }
    //cout<<h<<" , "<<s<<" , "<<l<<endl;

    if(l>=0.5){
        var1 = (l+s)-(l*s);
    }
    else if(l<0.5){
        var1 = l*(1.0+s);
    }
    var2 = 2.0*l - var1;
    barwa = h/360.0;
    //cout<<var1 << " " << var2<<endl;
    zmienaR=barwa+0.333;
    //cout<<zmienaR<<endl;
    zmienaG=barwa;
    //cout<<zmienaG<<endl;
    zmienaB=barwa - 0.333;
    //cout<<zmienaB<<endl;


    zmienaR=normalizeHsl(zmienaR);
    //cout<<zmienaR<<endl;
    zmienaG=normalizeHsl(zmienaG);
    //cout<<zmienaG<<endl;
    zmienaB=normalizeHsl(zmienaB);
    //cout<<zmienaB<<endl;

    r=test(zmienaR,var1,var2);
    //cout<<r<<endl;
    g=test(zmienaG,var1,var2);
    //cout<<g<<endl;
    b=test(zmienaB,var1,var2);
    //cout<<b<<endl;

    r*=255;
    //cout<<r<<endl;
    g*=255;
    //cout<<g<<endl;
    b*=255;
    //cout<<b<<endl;


    //cout<<r<<" , "<<g<<" , "<<b<<endl;

    setPixel(xx,yy,r,g,b);

}


float test(float color , float var1, float var2){
    float tempColor;
    if(6*color <1.0){
        tempColor = var2+(var1-var2)*6*color;
    }
    //test 2
    else if(2*color <1.0){
            tempColor = var1;
    }
    //test3
    else if(3*color<2.0){
            tempColor = var2 +(var1-var2) * (0.666- color)*6;
    }
    else if(3*color>=2.0){
        tempColor = var2;
    }
    return tempColor;
}

//P4
void subsample420_HSL(int width, int height)
{
    for (int y = 0; y < height; y += 2) {
        for (int x = 0; x < width; x += 2) {

            int x1 = (x + 1 < width)  ? x + 1 : x;
            int y1 = (y + 1 < height) ? y + 1 : y;

            HSL c00 = getHSL(x,  y );
            HSL c10 = getHSL(x1, y );
            HSL c01 = getHSL(x,  y1);
            HSL c11 = getHSL(x1, y1);

            float h_avg;
            {
                float sumX = cos(c00.h * M_PI / 180.0) +
                             cos(c10.h * M_PI / 180.0) +
                             cos(c01.h * M_PI / 180.0) +
                             cos(c11.h * M_PI / 180.0);

                float sumY = sin(c00.h * M_PI / 180.0) +
                             sin(c10.h * M_PI / 180.0) +
                             sin(c01.h * M_PI / 180.0) +
                             sin(c11.h * M_PI / 180.0);

                h_avg = atan2(sumY, sumX) * 180.0 / M_PI;
                if (h_avg < 0.0) h_avg += 360.0;
            }

            float s_avg = (c00.s + c10.s + c01.s + c11.s) / 4.0;


            setHSL(x+szerokosc/2,  y,  h_avg, s_avg, c00.l);
            setHSL(x1+szerokosc/2, y,  h_avg, s_avg, c10.l);
            setHSL(x+szerokosc/2,  y1, h_avg, s_avg, c01.l);
            setHSL(x1+szerokosc/2, y1, h_avg, s_avg, c11.l);
        }
    }
}



/////////////Lab3///////////////////////

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
void setRGB565(int xx, int yy, Uint8 r, Uint8 g, Uint8 b){
    //idk czy wejście to normalne czy już zmienione na 555
       int nowyR, nowyG, nowyB;
    Uint16 kolor16bit;
    nowyR = round(r*31.0/255.0);//5bit zakrs 0-31;
    nowyG = round(g*63.0/255.0);
    nowyB = round(b*31.0/255.0);

    kolor16bit = (nowyR<<11) | (nowyG<<5) | nowyB;
    setRGB565(xx,yy,kolor16bit);
}
void setRGB565(int xx, int yy, Uint16 rgb565){
   SDL_Color kolor24bit;
    int nowyR, nowyG, nowyB;
    nowyR = (rgb565&(0b1111100000000000))>>11;
    nowyG = (rgb565&(0b0000011111100000))>>5;
    nowyB = (rgb565&(0b0000000000011111));
    kolor24bit.r=nowyR*255.0/31.0;
    kolor24bit.g=nowyG*255.0/63.0;
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
SDL_Color getRGB565(int xx, int yy){
      Uint16 kolor16bit = getRGB565_(xx,yy);
    SDL_Color kolor24bit;
    int nowyR, nowyG, nowyB;
    nowyR = (kolor16bit&(0b1111100000000000))>>11;
    nowyG = (kolor16bit&(0b0000011111100000))>>6;
    nowyB = (kolor16bit&(0b0000000000011111));
    kolor24bit.r=nowyR*255.0/31.0;
    kolor24bit.g=nowyG*255.0/63.0;
    kolor24bit.b=nowyB*255.0/31.0;
    return kolor24bit;
}
Uint16 getRGB565_(int xx, int yy){
    SDL_Color kolor = getPixel(xx,yy);
    Uint16 kolor16bit;
    int nowyR, nowyG, nowyB;

    nowyR = round(kolor.r*31.0/255.0);//5bit zakrs 0-31;
    nowyG = round(kolor.g*63.0/255.0);
    nowyB = round(kolor.b*31.0/255.0);

    kolor16bit = (nowyR<<11) | (nowyG<<5) | nowyB;
    //bity: 15-11 r, 10-5 g, 4-0 b
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

// getRGB565D - odczyt z ditheringiem Bayera dla RGB565
SDL_Color getRGB565D(int xx, int yy){
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

    // Konwersja do RGB565 (5 bitów R, 6 bitów G, 5 bitów B)
    int nowyR = round(r * 31.0 / 255.0);
    int nowyG = round(g * 63.0 / 255.0);  // 6 bitów dla zielonego!
    int nowyB = round(b * 31.0 / 255.0);

    Uint16 kolor16bit = (nowyR << 11) | (nowyG << 5) | nowyB;

    // Konwersja z powrotem do 24-bit
    SDL_Color kolor24bit;
    nowyR = (kolor16bit & (0b1111100000000000)) >> 11;
    nowyG = (kolor16bit & (0b0000011111100000)) >> 5;
    nowyB = (kolor16bit & (0b0000000000011111));

    kolor24bit.r = nowyR * 255.0 / 31.0;
    kolor24bit.g = nowyG * 255.0 / 63.0;
    kolor24bit.b = nowyB * 255.0 / 31.0;

    return kolor24bit;
}

// getRGB565D_ - zwraca wartość Uint16 z ditheringiem
Uint16 getRGB565D_(int xx, int yy){
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

    // Konwersja do RGB565
    int nowyR = round(r * 31.0 / 255.0);
    int nowyG = round(g * 63.0 / 255.0);
    int nowyB = round(b * 31.0 / 255.0);

    Uint16 kolor16bit = (nowyR << 11) | (nowyG << 5) | nowyB;

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

void filtrujPNG_Typ1() {
    int width = szerokosc/2, height = wysokosc/2;
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            SDL_Color cur = getPixel(x,y);
            SDL_Color left = (x>0)? getPixel(x-1,y): SDL_Color{0,0,0};
            filtrPNG[x][y].r = (cur.r - left.r + 256)%256;
            filtrPNG[x][y].g = (cur.g - left.g + 256)%256;
            filtrPNG[x][y].b = (cur.b - left.b + 256)%256;
        }
    }
}

void filtrujPNG_Typ2() {
    int width = szerokosc/2, height = wysokosc/2;
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            SDL_Color cur = getPixel(x,y);
            SDL_Color up = (y>0)? getPixel(x,y-1): SDL_Color{0,0,0};
            filtrPNG[x][y].r = (cur.r - up.r + 256)%256;
            filtrPNG[x][y].g = (cur.g - up.g + 256)%256;
            filtrPNG[x][y].b = (cur.b - up.b + 256)%256;
        }
    }
}

void filtrujPNG_Typ3() {
    int width = szerokosc/2, height = wysokosc/2;
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            SDL_Color cur = getPixel(x,y);
            SDL_Color left = (x>0)? getPixel(x-1,y): SDL_Color{0,0,0};
            SDL_Color up   = (y>0)? getPixel(x,y-1): SDL_Color{0,0,0};
            filtrPNG[x][y].r = (cur.r - ((left.r+up.r)/2) + 256)%256;
            filtrPNG[x][y].g = (cur.g - ((left.g+up.g)/2) + 256)%256;
            filtrPNG[x][y].b = (cur.b - ((left.b+up.b)/2) + 256)%256;
        }
    }
}

void filtrujPNG_Typ4() {
    int width = szerokosc/2, height = wysokosc/2;
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            SDL_Color cur=getPixel(x,y);
            SDL_Color left=(x>0)? getPixel(x-1,y):SDL_Color{0,0,0};
            SDL_Color up=(y>0)? getPixel(x,y-1):SDL_Color{0,0,0};
            SDL_Color upLeft=(x>0 && y>0)? getPixel(x-1,y-1):SDL_Color{0,0,0};
            filtrPNG[x][y].r=(cur.r - paeth(left.r,up.r,upLeft.r)+256)%256;
            filtrPNG[x][y].g=(cur.g - paeth(left.g,up.g,upLeft.g)+256)%256;
            filtrPNG[x][y].b=(cur.b - paeth(left.b,up.b,upLeft.b)+256)%256;
        }
    }
}

//Odfiltrowywanie
void odfiltrujPNG_Typ1() {
    int width = szerokosc/2, height = wysokosc/2;
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            SDL_Color f = filtrPNG[x][y];
            SDL_Color left = (x>0)? getPixel(x-1,y+height): SDL_Color{0,0,0};
            SDL_Color out;
            out.r = (f.r + left.r)%256;
            out.g = (f.g + left.g)%256;
            out.b = (f.b + left.b)%256;
            setPixel(x,y+height,out.r,out.g,out.b);
        }
    }
}

void odfiltrujPNG_Typ2() {
    int width = szerokosc/2, height = wysokosc/2;
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            SDL_Color f = filtrPNG[x][y];
            SDL_Color up = (y>0)? getPixel(x,y+height-1): SDL_Color{0,0,0};
            SDL_Color out;
            out.r = (f.r + up.r)%256;
            out.g = (f.g + up.g)%256;
            out.b = (f.b + up.b)%256;
            setPixel(x,y+height,out.r,out.g,out.b);
        }
    }
}

void odfiltrujPNG_Typ3() {
    int width = szerokosc/2, height = wysokosc/2;
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            SDL_Color f = filtrPNG[x][y];
            SDL_Color left = (x>0)? getPixel(x-1,y+height): SDL_Color{0,0,0};
            SDL_Color up   = (y>0)? getPixel(x,y+height-1): SDL_Color{0,0,0};
            SDL_Color out;
            out.r = (f.r + ((left.r+up.r)/2))%256;
            out.g = (f.g + ((left.g+up.g)/2))%256;
            out.b = (f.b + ((left.b+up.b)/2))%256;
            setPixel(x,y+height,out.r,out.g,out.b);
        }
    }
}

void odfiltrujPNG_Typ4() {
    int width = szerokosc/2, height = wysokosc/2;
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            SDL_Color f = filtrPNG[x][y];
            SDL_Color left=(x>0)? getPixel(x-1,y+height):SDL_Color{0,0,0};
            SDL_Color up=(y>0)? getPixel(x,y+height-1):SDL_Color{0,0,0};
            SDL_Color upLeft=(x>0 && y>0)? getPixel(x-1,y+height-1):SDL_Color{0,0,0};
            SDL_Color out;
            out.r=(f.r + paeth(left.r,up.r,upLeft.r))%256;
            out.g=(f.g + paeth(left.g,up.g,upLeft.g))%256;
            out.b=(f.b + paeth(left.b,up.b,upLeft.b))%256;
            setPixel(x,y+height,out.r,out.g,out.b);
        }
    }
}

void filtrujYUV_Typ1() {
    int w = szerokosc / 2, h = wysokosc / 2;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            YUV c = buforYUV[x][y];
            YUV l = (x > 0) ? buforYUV[x-1][y] : YUV{0,0,0};
            filtrYUV[x][y].y = c.y - l.y;
            filtrYUV[x][y].u = c.u - l.u;
            filtrYUV[x][y].v = c.v - l.v;
        }
}

void filtrujYUV_Typ2() {
    int w = szerokosc / 2, h = wysokosc / 2;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            YUV c = buforYUV[x][y];
            YUV u = (y > 0) ? buforYUV[x][y-1] : YUV{0,0,0};
            filtrYUV[x][y].y = c.y - u.y;
            filtrYUV[x][y].u = c.u - u.u;
            filtrYUV[x][y].v = c.v - u.v;
        }
}

void filtrujYUV_Typ3() {
    int w = szerokosc / 2, h = wysokosc / 2;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            YUV c = buforYUV[x][y];
            YUV l = (x > 0) ? buforYUV[x-1][y] : YUV{0,0,0};
            YUV u = (y > 0) ? buforYUV[x][y-1] : YUV{0,0,0};
            filtrYUV[x][y].y = c.y - ((l.y + u.y) / 2.0f);
            filtrYUV[x][y].u = c.u - ((l.u + u.u) / 2.0f);
            filtrYUV[x][y].v = c.v - ((l.v + u.v) / 2.0f);
        }
}

void filtrujYUV_Typ4() {
    int w = szerokosc / 2, h = wysokosc / 2;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            YUV c  = buforYUV[x][y];
            YUV l  = (x > 0) ? buforYUV[x-1][y] : YUV{0,0,0};
            YUV u  = (y > 0) ? buforYUV[x][y-1] : YUV{0,0,0};
            YUV ul = (x > 0 && y > 0) ? buforYUV[x-1][y-1] : YUV{0,0,0};
            filtrYUV[x][y].y = c.y - paeth(l.y, u.y, ul.y);
            filtrYUV[x][y].u = c.u - paeth(l.u, u.u, ul.u);
            filtrYUV[x][y].v = c.v - paeth(l.v, u.v, ul.v);
        }
}

// ======== ODFILTROWYWANIE ========

void odfiltrujYUV_Typ1() {
    int w = szerokosc / 2, h = wysokosc / 2;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            YUV f = filtrYUV[x][y];
            YUV l = (x > 0) ? buforYUV[x-1][y] : YUV{0,0,0};

            YUV nowy;
            nowy.y = f.y + l.y;
            nowy.u = f.u + l.u;
            nowy.v = f.v + l.v;
            buforYUV[x][y] = nowy;

            setYUV(x + w, y, nowy.y, nowy.u, nowy.v);
        }
}

void odfiltrujYUV_Typ2() {
    int w = szerokosc / 2, h = wysokosc / 2;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            YUV f = filtrYUV[x][y];
            YUV u = (y > 0) ? buforYUV[x][y-1] : YUV{0,0,0};

            YUV nowy;
            nowy.y = f.y + u.y;
            nowy.u = f.u + u.u;
            nowy.v = f.v + u.v;
            buforYUV[x][y] = nowy;

            setYUV(x + w, y, nowy.y, nowy.u, nowy.v);
        }
}

void odfiltrujYUV_Typ3() {
    int w = szerokosc / 2, h = wysokosc / 2;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            YUV f = filtrYUV[x][y];
            YUV l = (x > 0) ? buforYUV[x-1][y] : YUV{0,0,0};
            YUV u = (y > 0) ? buforYUV[x][y-1] : YUV{0,0,0};

            YUV nowy;
            nowy.y = f.y + ((l.y + u.y) / 2.0f);
            nowy.u = f.u + ((l.u + u.u) / 2.0f);
            nowy.v = f.v + ((l.v + u.v) / 2.0f);
            buforYUV[x][y] = nowy;

            setYUV(x + w, y, nowy.y, nowy.u, nowy.v);
        }
}

void odfiltrujYUV_Typ4() {
    int w = szerokosc / 2, h = wysokosc / 2;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            YUV f  = filtrYUV[x][y];
            YUV l  = (x > 0) ? buforYUV[x-1][y] : YUV{0,0,0};
            YUV u  = (y > 0) ? buforYUV[x][y-1] : YUV{0,0,0};
            YUV ul = (x > 0 && y > 0) ? buforYUV[x-1][y-1] : YUV{0,0,0};

            YUV nowy;
            nowy.y = f.y + paeth(l.y, u.y, ul.y);
            nowy.u = f.u + paeth(l.u, u.u, ul.u);
            nowy.v = f.v + paeth(l.v, u.v, ul.v);
            buforYUV[x][y] = nowy;

            setYUV(x + w, y, nowy.y, nowy.u, nowy.v);
        }
}

void filtrujRGB555_Typ1(){
    int w=szerokosc/2, h=wysokosc/2;
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            Uint16 cur=getRGB555_(x,y);
            Uint16 L = (x>0)? getRGB555_(x-1,y):0;

            Uint8 cL =  cur &0xFF, cH = (cur>>8)&0xFF;
            Uint8 lL =  L &0xFF, lH = (L  >>8)&0xFF;

            filtrRGB555_lo[x][y] = (Uint8)((cL - lL + 256)%256);
            filtrRGB555_hi[x][y] = (Uint8)((cH - lH + 256)%256);
        }
    }
}

void odfiltrujRGB555_Typ1(){
    int w=szerokosc/2, h=wysokosc/2;
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            Uint8 fL=filtrRGB555_lo[x][y], fH=filtrRGB555_hi[x][y];

            Uint16 L = (x>0)? getRGB555_(x-1,y+h):0;
            Uint8 lL=L&0xFF, lH=(L>>8)&0xFF;

            Uint8 nL=(Uint8)((fL + lL)%256);
            Uint8 nH=(Uint8)((fH + lH)%256);

            Uint16 out=(Uint16)nL | ((Uint16)nH<<8);
            setRGB555(x,y+h,out);
        }
    }
}

void filtrujRGB555_Typ2(){
    int w=szerokosc/2, h=wysokosc/2;
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            Uint16 cur=getRGB555_(x,y);
            Uint16 U = (y>0)? getRGB555_(x,y-1):0;

            Uint8 cL=cur&0xFF, cH=(cur>>8)&0xFF;
            Uint8 uL=U  &0xFF, uH=(U  >>8)&0xFF;

            filtrRGB555_lo[x][y]=(Uint8)((cL - uL + 256)%256);
            filtrRGB555_hi[x][y]=(Uint8)((cH - uH + 256)%256);
        }
    }
}

void odfiltrujRGB555_Typ2(){
    int w=szerokosc/2, h=wysokosc/2;
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            Uint8 fL=filtrRGB555_lo[x][y], fH=filtrRGB555_hi[x][y];

            Uint16 U=(y>0)? getRGB555_(x,y+h-1):0;
            Uint8 uL=U&0xFF, uH=(U>>8)&0xFF;

            Uint8 nL=(Uint8)((fL + uL)%256);
            Uint8 nH=(Uint8)((fH + uH)%256);

            Uint16 out=(Uint16)nL | ((Uint16)nH<<8);
            setRGB555(x,y+h,out);
        }
    }
}

void filtrujRGB555_Typ3(){
    int w=szerokosc/2, h=wysokosc/2;
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
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
    int w=szerokosc/2, h=wysokosc/2;
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            Uint8 fL=filtrRGB555_lo[x][y], fH=filtrRGB555_hi[x][y];

            Uint16 L=(x>0)?getRGB555_(x-1,y+h):0;
            Uint16 U=(y>0)?getRGB555_(x,y+h-1):0;

            Uint8 lL=L&0xFF, lH=(L>>8)&0xFF;
            Uint8 uL=U&0xFF, uH=(U>>8)&0xFF;

            Uint8 avgL=(Uint8)(((int)lL + (int)uL)/2);
            Uint8 avgH=(Uint8)(((int)lH + (int)uH)/2);

            Uint8 nL=(Uint8)((fL + avgL)%256);
            Uint8 nH=(Uint8)((fH + avgH)%256);

            Uint16 out=(Uint16)nL | ((Uint16)nH<<8);
            setRGB555(x,y+h,out);
        }
    }
}

void filtrujRGB555_Typ4(){
    int w=szerokosc/2, h=wysokosc/2;
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            Uint16 cur=getRGB555_(x,y);
            Uint16 L=(x>0)?getRGB555_(x-1,y):0;
            Uint16 U=(y>0)?getRGB555_(x,y-1):0;
            Uint16 UL=(x>0&&y>0)?getRGB555_(x-1,y-1):0;

            Uint8 cL=cur&0xFF, cH=(cur>>8)&0xFF;
            Uint8 lL=L&0xFF,   lH=(L>>8)&0xFF;
            Uint8 uL=U&0xFF,   uH=(U>>8)&0xFF;
            Uint8 ulL=UL&0xFF, ulH=(UL>>8)&0xFF;

            filtrRGB555_lo[x][y]=(Uint8)((cL - paeth(lL,uL,ulL) + 256)%256);
            filtrRGB555_hi[x][y]=(Uint8)((cH - paeth(lH,uH,ulH) + 256)%256);
        }
    }
}

void odfiltrujRGB555_Typ4(){
    int w=szerokosc/2, h=wysokosc/2;
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            Uint8 fL=filtrRGB555_lo[x][y], fH=filtrRGB555_hi[x][y];

            Uint16 L=(x>0)?getRGB555_(x-1,y+h):0;
            Uint16 U=(y>0)?getRGB555_(x,y+h-1):0;
            Uint16 UL=(x>0&&y>0)?getRGB555_(x-1,y+h-1):0;

            Uint8 lL=L&0xFF, lH=(L>>8)&0xFF;
            Uint8 uL=U&0xFF, uH=(U>>8)&0xFF;
            Uint8 ulL=UL&0xFF, ulH=(UL>>8)&0xFF;

            Uint8 nL=(Uint8)((fL + paeth(lL,uL,ulL))%256);
            Uint8 nH=(Uint8)((fH + paeth(lH,uH,ulH))%256);

            Uint16 out=(Uint16)nL | ((Uint16)nH<<8);
            setRGB555(x,y+h,out);
        }
    }
}
void filtrujRGB565_Typ1(){
    int w=szerokosc/2, h=wysokosc/2;
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            Uint16 cur=getRGB565_(x,y);
            Uint16 L = (x>0)? getRGB565_(x-1,y):0;

            Uint8 cL=cur&0xFF, cH=(cur>>8)&0xFF;
            Uint8 lL=L  &0xFF, lH=(L  >>8)&0xFF;

            filtrRGB565_lo[x][y]=(Uint8)((cL - lL + 256)%256);
            filtrRGB565_hi[x][y]=(Uint8)((cH - lH + 256)%256);
        }
    }
}

void odfiltrujRGB565_Typ1(){
    int w=szerokosc/2, h=wysokosc/2;
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            Uint8 fL=filtrRGB565_lo[x][y], fH=filtrRGB565_hi[x][y];

            Uint16 L=(x>0)?getRGB565_(x-1+w,y+h):0;
            Uint8 lL=L&0xFF, lH=(L>>8)&0xFF;

            Uint8 nL=(Uint8)((fL + lL)%256);
            Uint8 nH=(Uint8)((fH + lH)%256);

            Uint16 out=(Uint16)nL | ((Uint16)nH<<8);
            setRGB565(x+w,y+h,out);
        }
    }
}

void filtrujRGB565_Typ2(){
    int w=szerokosc/2, h=wysokosc/2;
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            Uint16 cur=getRGB565_(x,y);
            Uint16 U=(y>0)?getRGB565_(x,y-1):0;

            Uint8 cL=cur&0xFF, cH=(cur>>8)&0xFF;
            Uint8 uL=U  &0xFF, uH=(U  >>8)&0xFF;

            filtrRGB565_lo[x][y]=(Uint8)((cL - uL + 256)%256);
            filtrRGB565_hi[x][y]=(Uint8)((cH - uH + 256)%256);
        }
    }
}

void odfiltrujRGB565_Typ2(){
    int w=szerokosc/2, h=wysokosc/2;
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            Uint8 fL=filtrRGB565_lo[x][y], fH=filtrRGB565_hi[x][y];

            Uint16 U=(y>0)?getRGB565_(x+w,y+h-1):0;
            Uint8 uL=U&0xFF, uH=(U>>8)&0xFF;

            Uint8 nL=(Uint8)((fL + uL)%256);
            Uint8 nH=(Uint8)((fH + uH)%256);

            Uint16 out=(Uint16)nL | ((Uint16)nH<<8);
            setRGB565(x+w,y+h,out);
        }
    }
}

void filtrujRGB565_Typ3(){
    int w=szerokosc/2, h=wysokosc/2;
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            Uint16 cur=getRGB565_(x,y);
            Uint16 L=(x>0)?getRGB565_(x-1,y):0;
            Uint16 U=(y>0)?getRGB565_(x,y-1):0;

            Uint8 cL=cur&0xFF, cH=(cur>>8)&0xFF;
            Uint8 lL=L  &0xFF, lH=(L  >>8)&0xFF;
            Uint8 uL=U  &0xFF, uH=(U  >>8)&0xFF;

            Uint8 avgL=(Uint8)(((int)lL + (int)uL)/2);
            Uint8 avgH=(Uint8)(((int)lH + (int)uH)/2);

            filtrRGB565_lo[x][y]=(Uint8)((cL - avgL + 256)%256);
            filtrRGB565_hi[x][y]=(Uint8)((cH - avgH + 256)%256);
        }
    }
}

void odfiltrujRGB565_Typ3(){
    int w=szerokosc/2, h=wysokosc/2;
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            Uint8 fL=filtrRGB565_lo[x][y], fH=filtrRGB565_hi[x][y];

            Uint16 L=(x>0)?getRGB565_(x-1+w,y+h):0;
            Uint16 U=(y>0)?getRGB565_(x+w,y+h-1):0;

            Uint8 lL=L&0xFF, lH=(L>>8)&0xFF;
            Uint8 uL=U&0xFF, uH=(U>>8)&0xFF;

            Uint8 avgL=(Uint8)(((int)lL + (int)uL)/2);
            Uint8 avgH=(Uint8)(((int)lH + (int)uH)/2);

            Uint8 nL=(Uint8)((fL + avgL)%256);
            Uint8 nH=(Uint8)((fH + avgH)%256);

            Uint16 out=(Uint16)nL | ((Uint16)nH<<8);
            setRGB565(x+w,y+h,out);
        }
    }
}


void filtrujRGB565_Typ4(){
    int w=szerokosc/2, h=wysokosc/2;
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            Uint16 cur=getRGB565_(x,y);
            Uint16 L=(x>0)?getRGB565_(x-1,y):0;
            Uint16 U=(y>0)?getRGB565_(x,y-1):0;
            Uint16 UL=(x>0&&y>0)?getRGB565_(x-1,y-1):0;

            Uint8 cL=cur&0xFF, cH=(cur>>8)&0xFF;
            Uint8 lL=L&0xFF,   lH=(L>>8)&0xFF;
            Uint8 uL=U&0xFF,   uH=(U>>8)&0xFF;
            Uint8 ulL=UL&0xFF, ulH=(UL>>8)&0xFF;

            filtrRGB565_lo[x][y]=(Uint8)((cL - paeth(lL,uL,ulL) + 256)%256);
            filtrRGB565_hi[x][y]=(Uint8)((cH - paeth(lH,uH,ulH) + 256)%256);
        }
    }
}

void odfiltrujRGB565_Typ4(){
    int w=szerokosc/2, h=wysokosc/2;
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            Uint8 fL=filtrRGB565_lo[x][y], fH=filtrRGB565_hi[x][y];

            Uint16 L=(x>0)?getRGB565_(x-1+w,y+h):0;
            Uint16 U=(y>0)?getRGB565_(x+w,y+h-1):0;
            Uint16 UL=(x>0&&y>0)?getRGB565_(x-1+w,y+h-1):0;

            Uint8 lL=L&0xFF, lH=(L>>8)&0xFF;
            Uint8 uL=U&0xFF, uH=(U>>8)&0xFF;
            Uint8 ulL=UL&0xFF, ulH=(UL>>8)&0xFF;

            Uint8 nL=(Uint8)((fL + paeth(lL,uL,ulL))%256);
            Uint8 nH=(Uint8)((fH + paeth(lH,uH,ulH))%256);

            Uint16 out=(Uint16)nL | ((Uint16)nH<<8);
            setRGB565(x+w,y+h,out);
        }
    }
}
void zapiszYUVDoBufora() {
    int w = szerokosc / 2;
    int h = wysokosc / 2;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            buforYUV[x][y] = getYUV(x, y);
        }
    }
}

void filtrujYUV_Optymalnie() {
    int w = szerokosc / 2, h = wysokosc / 2;

    for (int y = 0; y < h; y++) {
        double najlepszaSuma = 100000000.0;
        int najlepszyTyp = 0;

        // Testuj wszystkie typy 0–4
        for (int typ = 0; typ <= 4; typ++) {
            double suma = 0.0;

            for (int x = 0; x < w; x++) {
                YUV c  = buforYUV[x][y];
                YUV l  = (x > 0) ? buforYUV[x-1][y] : YUV{0,0,0};
                YUV u  = (y > 0) ? buforYUV[x][y-1] : YUV{0,0,0};
                YUV ul = (x > 0 && y > 0) ? buforYUV[x-1][y-1] : YUV{0,0,0};

                double predY, predU, predV;

                switch (typ) {
                    case 0: predY=predU=predV=0; break;
                    case 1: predY=l.y; predU=l.u; predV=l.v; break;
                    case 2: predY=u.y; predU=u.u; predV=u.v; break;
                    case 3:
                        predY=floor((l.y+u.y)/2.0);
                        predU=floor((l.u+u.u)/2.0);
                        predV=floor((l.v+u.v)/2.0);
                        break;
                    case 4:
                        predY=paeth(l.y,u.y,ul.y);
                        predU=paeth(l.u,u.u,ul.u);
                        predV=paeth(l.v,u.v,ul.v);
                        break;
                }

                double dy = c.y - predY;
                double du = c.u - predU;
                double dv = c.v - predV;

                suma += fabs(dy) + fabs(du) + fabs(dv);
            }

            if (suma < najlepszaSuma) {
                najlepszaSuma = suma;
                najlepszyTyp = typ;
                filtrTypYUV[y] = najlepszyTyp;
            }
        }

        // Drugi przebieg — zapis najlepszego typu
        for (int x = 0; x < w; x++) {
            YUV c  = buforYUV[x][y];
            YUV l  = (x > 0) ? buforYUV[x-1][y] : YUV{0,0,0};
            YUV u  = (y > 0) ? buforYUV[x][y-1] : YUV{0,0,0};
            YUV ul = (x > 0 && y > 0) ? buforYUV[x-1][y-1] : YUV{0,0,0};

            double predY, predU, predV;

            switch (najlepszyTyp) {
                case 0: predY=predU=predV=0; break;
                case 1: predY=l.y; predU=l.u; predV=l.v; break;
                case 2: predY=u.y; predU=u.u; predV=u.v; break;
                case 3:
                    predY=floor((l.y+u.y)/2.0);
                    predU=floor((l.u+u.u)/2.0);
                    predV=floor((l.v+u.v)/2.0);
                    break;
                case 4:
                    predY=paeth(l.y,u.y,ul.y);
                    predU=paeth(l.u,u.u,ul.u);
                    predV=paeth(l.v,u.v,ul.v);
                    break;
            }

            filtrYUV[x][y].y = c.y - predY;
            filtrYUV[x][y].u = c.u - predU;
            filtrYUV[x][y].v = c.v - predV;
        }
    }
}

void filtrujPNG_Optymalnie() {
    int width = szerokosc/2, height = wysokosc/2;

    for (int y = 0; y < height; y++) {
        long najlepszaSuma = LONG_MAX;
        int najlepszyTyp = 0;

        // Tablica pomocnicza na wyniki próbne
        SDL_Color tymczasowy[320][1]; // tylko 1 linia na raz

        // Przetestuj każdy typ filtra
        for (int typ = 0; typ <= 4; typ++) {
            long suma = 0;

            for (int x = 0; x < width; x++) {
                SDL_Color cur = getPixel(x,y);
                SDL_Color left = (x>0)? getPixel(x-1,y): SDL_Color{0,0,0};
                SDL_Color up   = (y>0)? getPixel(x,y-1): SDL_Color{0,0,0};
                SDL_Color upLeft = (x>0 && y>0)? getPixel(x-1,y-1): SDL_Color{0,0,0};

                int predR, predG, predB;
                switch (typ) {
                    case 0: predR=predG=predB=0; break;
                    case 1: predR=left.r; predG=left.g; predB=left.b; break;
                    case 2: predR=up.r; predG=up.g; predB=up.b; break;
                    case 3:
                        predR=floor((left.r+up.r)/2.0);
                        predG=floor((left.g+up.g)/2.0);
                        predB=floor((left.b+up.b)/2.0);
                        break;
                    case 4:
                        predR=paeth(left.r,up.r,upLeft.r);
                        predG=paeth(left.g,up.g,upLeft.g);
                        predB=paeth(left.b,up.b,upLeft.b);
                        break;
                }

                int dr = (cur.r - predR + 256) % 256;
                int dg = (cur.g - predG + 256) % 256;
                int db = (cur.b - predB + 256) % 256;

                suma += abs(dr-128) + abs(dg-128) + abs(db-128);
                tymczasowy[x][0] = {Uint8(dr), Uint8(dg), Uint8(db)};
            }

            if (suma < najlepszaSuma) {
                najlepszaSuma = suma;
                najlepszyTyp = typ;
                filtrTypPNG[y] = najlepszyTyp;

            }
        }

        // Drugi przebieg: zapisz wyniki najlepszego typu do filtrPNG
        for (int x = 0; x < width; x++) {
            SDL_Color cur = getPixel(x,y);
            SDL_Color left = (x>0)? getPixel(x-1,y): SDL_Color{0,0,0};
            SDL_Color up   = (y>0)? getPixel(x,y-1): SDL_Color{0,0,0};
            SDL_Color upLeft = (x>0 && y>0)? getPixel(x-1,y-1): SDL_Color{0,0,0};

            int predR, predG, predB;
            switch (najlepszyTyp) {
                case 0: predR=predG=predB=0; break;
                case 1: predR=left.r; predG=left.g; predB=left.b; break;
                case 2: predR=up.r; predG=up.g; predB=up.b; break;
                case 3:
                    predR=floor((left.r+up.r)/2.0);
                    predG=floor((left.g+up.g)/2.0);
                    predB=floor((left.b+up.b)/2.0);
                    break;
                case 4:
                    predR=paeth(left.r,up.r,upLeft.r);
                    predG=paeth(left.g,up.g,upLeft.g);
                    predB=paeth(left.b,up.b,upLeft.b);
                    break;
            }

            filtrPNG[x][y].r = (cur.r - predR + 256)%256;
            filtrPNG[x][y].g = (cur.g - predG + 256)%256;
            filtrPNG[x][y].b = (cur.b - predB + 256)%256;
        }
    }
}

void filtrujRGB555_Optymalnie() {
    int w = szerokosc / 2, h = wysokosc / 2;

    for (int y = 0; y < h; y++) {
        double najlepszaSuma = 100000000.0;
        int najlepszyTyp = 0;

        for (int typ = 0; typ <= 4; typ++) {
            double suma = 0.0;

            for (int x = 0; x < w; x++) {
                Uint16 cur = getRGB555_(x,y);
                Uint16 L = (x>0)? getRGB555_(x-1,y):0;
                Uint16 U = (y>0)? getRGB555_(x,y-1):0;
                Uint16 UL= (x>0 && y>0)? getRGB555_(x-1,y-1):0;

                Uint8 cL = cur & 0xFF, cH = (cur>>8)&0xFF;
                Uint8 lL = L & 0xFF,   lH = (L>>8)&0xFF;
                Uint8 uL = U & 0xFF,   uH = (U>>8)&0xFF;
                Uint8 ulL= UL & 0xFF,  ulH= (UL>>8)&0xFF;

                int pL=0, pH=0;
                switch (typ) {
                    case 0: pL=pH=0; break;
                    case 1: pL=lL; pH=lH; break;
                    case 2: pL=uL; pH=uH; break;
                    case 3: pL=floor((lL+uL)/2.0); pH=floor((lH+uH)/2.0); break;
                    case 4: pL=paeth(lL,uL,ulL); pH=paeth(lH,uH,ulH); break;
                }

                suma += fabs((double)((cL - pL + 256)%256 - 128)) +
                        fabs((double)((cH - pH + 256)%256 - 128));
            }

            if (suma < najlepszaSuma) {
                najlepszaSuma = suma;
                najlepszyTyp = typ;
                filtrTypRGB555[y] = najlepszyTyp;

            }
        }

        // Drugi przebieg — zapisz wynik
        for (int x = 0; x < w; x++) {
            Uint16 cur = getRGB555_(x,y);
            Uint16 L = (x>0)? getRGB555_(x-1,y):0;
            Uint16 U = (y>0)? getRGB555_(x,y-1):0;
            Uint16 UL= (x>0 && y>0)? getRGB555_(x-1,y-1):0;

            Uint8 cL = cur & 0xFF, cH = (cur>>8)&0xFF;
            Uint8 lL = L & 0xFF,   lH = (L>>8)&0xFF;
            Uint8 uL = U & 0xFF,   uH = (U>>8)&0xFF;
            Uint8 ulL= UL & 0xFF,  ulH= (UL>>8)&0xFF;

            int pL=0, pH=0;
            switch (najlepszyTyp) {
                case 0: pL=pH=0; break;
                case 1: pL=lL; pH=lH; break;
                case 2: pL=uL; pH=uH; break;
                case 3: pL=floor((lL+uL)/2.0); pH=floor((lH+uH)/2.0); break;
                case 4: pL=paeth(lL,uL,ulL); pH=paeth(lH,uH,ulH); break;
            }

            filtrRGB555_lo[x][y] = (Uint8)((cL - pL + 256)%256);
            filtrRGB555_hi[x][y] = (Uint8)((cH - pH + 256)%256);
        }
    }
}

void filtrujRGB565_Optymalnie() {
    int w = szerokosc / 2, h = wysokosc / 2;

    for (int y = 0; y < h; y++) {
        double najlepszaSuma = 100000000.0;
        int najlepszyTyp = 0;

        for (int typ = 0; typ <= 4; typ++) {
            double suma = 0.0;

            for (int x = 0; x < w; x++) {
                Uint16 cur = getRGB565_(x,y);
                Uint16 L = (x>0)? getRGB565_(x-1,y):0;
                Uint16 U = (y>0)? getRGB565_(x,y-1):0;
                Uint16 UL= (x>0 && y>0)? getRGB565_(x-1,y-1):0;

                Uint8 cL = cur & 0xFF, cH = (cur>>8)&0xFF;
                Uint8 lL = L & 0xFF,   lH = (L>>8)&0xFF;
                Uint8 uL = U & 0xFF,   uH = (U>>8)&0xFF;
                Uint8 ulL= UL & 0xFF,  ulH= (UL>>8)&0xFF;

                int pL=0, pH=0;
                switch (typ) {
                    case 0: pL=pH=0; break;
                    case 1: pL=lL; pH=lH; break;
                    case 2: pL=uL; pH=uH; break;
                    case 3: pL=floor((lL+uL)/2.0); pH=floor((lH+uH)/2.0); break;
                    case 4: pL=paeth(lL,uL,ulL); pH=paeth(lH,uH,ulH); break;
                }

                suma += fabs((double)((cL - pL + 256)%256 - 128)) +
                        fabs((double)((cH - pH + 256)%256 - 128));
            }

            if (suma < najlepszaSuma) {
                najlepszaSuma = suma;
                najlepszyTyp = typ;
                filtrTypRGB565[y] = najlepszyTyp;

            }
        }

        // Zapis najlepszego typu
        for (int x = 0; x < w; x++) {
            Uint16 cur = getRGB565_(x,y);
            Uint16 L = (x>0)? getRGB565_(x-1,y):0;
            Uint16 U = (y>0)? getRGB565_(x,y-1):0;
            Uint16 UL= (x>0 && y>0)? getRGB565_(x-1,y-1):0;

            Uint8 cL = cur & 0xFF, cH = (cur>>8)&0xFF;
            Uint8 lL = L & 0xFF,   lH = (L>>8)&0xFF;
            Uint8 uL = U & 0xFF,   uH = (U>>8)&0xFF;
            Uint8 ulL= UL & 0xFF,  ulH= (UL>>8)&0xFF;

            int pL=0, pH=0;
            switch (najlepszyTyp) {
                case 0: pL=pH=0; break;
                case 1: pL=lL; pH=lH; break;
                case 2: pL=uL; pH=uH; break;
                case 3: pL=floor((lL+uL)/2.0); pH=floor((lH+uH)/2.0); break;
                case 4: pL=paeth(lL,uL,ulL); pH=paeth(lH,uH,ulH); break;
            }

            filtrRGB565_lo[x][y] = (Uint8)((cL - pL + 256)%256);
            filtrRGB565_hi[x][y] = (Uint8)((cH - pH + 256)%256);
        }
    }
}

void odfiltrujPNG_Optymalnie() {
    int width = szerokosc/2, height = wysokosc/2;

    for (int y = 0; y < height; y++) {
        int typ = filtrTypPNG[y]; // typ wybrany przy filtracji

        for (int x = 0; x < width; x++) {
            SDL_Color f = filtrPNG[x][y];
            SDL_Color left = (x>0)? getPixel(x-1, y+height): SDL_Color{0,0,0};
            SDL_Color up   = (y>0)? getPixel(x, y+height-1): SDL_Color{0,0,0};
            SDL_Color upLeft = (x>0 && y>0)? getPixel(x-1, y+height-1): SDL_Color{0,0,0};

            SDL_Color out;
            switch (typ) {
                case 0: // None
                    out = f;
                    break;
                case 1: // Sub
                    out.r = (f.r + left.r) % 256;
                    out.g = (f.g + left.g) % 256;
                    out.b = (f.b + left.b) % 256;
                    break;
                case 2: // Up
                    out.r = (f.r + up.r) % 256;
                    out.g = (f.g + up.g) % 256;
                    out.b = (f.b + up.b) % 256;
                    break;
                case 3: // Average
                    out.r = ((int)f.r + (int)floor((left.r + up.r) / 2.0)) % 256;
                    out.g = ((int)f.g + (int)floor((left.g + up.g) / 2.0)) % 256;
                    out.b = ((int)f.b + (int)floor((left.b + up.b) / 2.0)) % 256;
                    break;
                case 4: // Paeth
                    out.r = (f.r + paeth(left.r, up.r, upLeft.r)) % 256;
                    out.g = (f.g + paeth(left.g, up.g, upLeft.g)) % 256;
                    out.b = (f.b + paeth(left.b, up.b, upLeft.b)) % 256;
                    break;
            }
            setPixel(x, y+height, out.r, out.g, out.b);
        }
    }
}
void odfiltrujYUV_Optymalnie() {
    int w = szerokosc/2, h = wysokosc/2;

    for (int y = 0; y < h; y++) {
        int typ = filtrTypYUV[y];
        cout<<typ<<"\n";
        for (int x = 0; x < w; x++) {
            YUV f = filtrYUV[x][y];
            YUV l  = (x>0)? buforYUV[x-1][y]: YUV{0,0,0};
            YUV u  = (y>0)? buforYUV[x][y-1]: YUV{0,0,0};
            YUV ul = (x>0 && y>0)? buforYUV[x-1][y-1]: YUV{0,0,0};

            YUV nowy;
            switch (typ) {
                case 0:
                    nowy = f;
                    break;
                case 1:
                    nowy.y = f.y + l.y;
                    nowy.u = f.u + l.u;
                    nowy.v = f.v + l.v;
                    break;
                case 2:
                    nowy.y = f.y + u.y;
                    nowy.u = f.u + u.u;
                    nowy.v = f.v + u.v;
                    break;
                case 3:
                    nowy.y = f.y + floor((l.y + u.y)/2.0);
                    nowy.u = f.u + floor((l.u + u.u)/2.0);
                    nowy.v = f.v + floor((l.v + u.v)/2.0);
                    break;
                case 4:
                    nowy.y = f.y + paeth(l.y,u.y,ul.y);
                    nowy.u = f.u + paeth(l.u,u.u,ul.u);
                    nowy.v = f.v + paeth(l.v,u.v,ul.v);
                    break;
            }

            buforYUV[x][y] = nowy;
            setYUV(x+w, y, nowy.y, nowy.u, nowy.v);
        }
    }
}
void odfiltrujRGB555_Optymalnie() {
    int w = szerokosc/2, h = wysokosc/2;

    for (int y = 0; y < h; y++) {
        int typ = filtrTypRGB555[y];

        for (int x = 0; x < w; x++) {
            Uint8 fL = filtrRGB555_lo[x][y];
            Uint8 fH = filtrRGB555_hi[x][y];

            Uint16 L  = (x>0)? getRGB555_(x-1,y+h):0;
            Uint16 U  = (y>0)? getRGB555_(x,y+h-1):0;
            Uint16 UL = (x>0&&y>0)? getRGB555_(x-1,y+h-1):0;

            Uint8 lL=L&0xFF, lH=(L>>8)&0xFF;
            Uint8 uL=U&0xFF, uH=(U>>8)&0xFF;
            Uint8 ulL=UL&0xFF, ulH=(UL>>8)&0xFF;

            Uint8 nL,nH;
            switch(typ) {
                case 0:
                    nL=fL; nH=fH; break;
                case 1:
                    nL=(fL + lL)%256; nH=(fH + lH)%256; break;
                case 2:
                    nL=(fL + uL)%256; nH=(fH + uH)%256; break;
                case 3:
                    nL=((int)fL + (int)floor((lL+uL)/2.0))%256;
                    nH=((int)fH + (int)floor((lH+uH)/2.0))%256;
                    break;
                case 4:
                    nL=(fL + paeth(lL,uL,ulL))%256;
                    nH=(fH + paeth(lH,uH,ulH))%256;
                    break;
            }

            Uint16 out=(Uint16)nL | ((Uint16)nH<<8);
            setRGB555(x,y+h,out);
        }
    }
}
void odfiltrujRGB565_Optymalnie() {
    int w = szerokosc/2, h = wysokosc/2;

    for (int y = 0; y < h; y++) {
        int typ = filtrTypRGB565[y];

        for (int x = 0; x < w; x++) {
            Uint8 fL = filtrRGB565_lo[x][y];
            Uint8 fH = filtrRGB565_hi[x][y];

            Uint16 L  = (x>0)? getRGB565_(x-1+w,y+h):0;
            Uint16 U  = (y>0)? getRGB565_(x+w,y+h-1):0;
            Uint16 UL = (x>0&&y>0)? getRGB565_(x-1+w,y+h-1):0;

            Uint8 lL=L&0xFF, lH=(L>>8)&0xFF;
            Uint8 uL=U&0xFF, uH=(U>>8)&0xFF;
            Uint8 ulL=UL&0xFF, ulH=(UL>>8)&0xFF;

            Uint8 nL,nH;
            switch(typ) {
                case 0:
                    nL=fL; nH=fH; break;
                case 1:
                    nL=(fL + lL)%256; nH=(fH + lH)%256; break;
                case 2:
                    nL=(fL + uL)%256; nH=(fH + uH)%256; break;
                case 3:
                    nL=((int)fL + (int)floor((lL+uL)/2.0))%256;
                    nH=((int)fH + (int)floor((lH+uH)/2.0))%256;
                    break;
                case 4:
                    nL=(fL + paeth(lL,uL,ulL))%256;
                    nH=(fH + paeth(lH,uH,ulH))%256;
                    break;
            }

            Uint16 out=(Uint16)nL | ((Uint16)nH<<8);
            setRGB565(x+w,y+h,out);
        }
    }
}

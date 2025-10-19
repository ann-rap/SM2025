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



YUV getYUV(int xx, int yy){
    SDL_Color bit8color  = getPixel(xx,yy);
    SDL_Color kolor = z6Kdo24K(z24Kdo6K(bit8color));
    YUV newColor;
    float r = kolor.r;
    float g = kolor.g;
    float b = kolor.b;
    newColor.y = (0.299 * r) + (0.587 * g) + (0.114 * b);
    newColor.u = (-0.14713 * r) + (-0.28886 * g) + (0.436 * b);
    newColor.v = (0.615 * r) + (-0.51499 * g) + (-0.10001 * b);

    return newColor;
}

YIQ getYIQ(int xx,int yy){
    SDL_Color bit8color  = getPixel(xx,yy);
    SDL_Color kolor = z6Kdo24K(z24Kdo6K(bit8color));
    YIQ newColor;
    float r = kolor.r;
    float g = kolor.g;
    float b = kolor.b;
    newColor.y = (0.299 * r) + (0.587 *g) + (0.114*b);
    newColor.i = (0.5959 * r) + (-0.2746 *g) + (-0.3213*b);
    newColor.q = (0.2115 * r) + (-0.5227 *g) + (0.3112*b);


    return newColor;

}
YCbCr getYCbCr(int xx, int yy){
    SDL_Color bit8color  = getPixel(xx,yy);
    SDL_Color kolor = z6Kdo24K(z24Kdo6K(bit8color));

    YCbCr nowyKolor;
    float r = kolor.r;
    float g = kolor.g;
    float b = kolor.b;
    nowyKolor.y = 0 + (0.299 * r) + (0.587 * g) + (0.114 * b);
    nowyKolor.cb = 128 - (0.168736 * r) - (0.331264 * g) + (0.5 * b);
    nowyKolor.cr = 128 + (0.5 * r) - (0.418688 * g) - (0.081312 * b);

    return nowyKolor;
}


void setYUV(int xx, int yy, float y, float u, float v){
    float r = y + (0 * u) + (1.13983 * v);
    float g = y + (-0.39465 * u) + (-0.58060 * v);
    float b = y + (2.03211 * u) + (0 * v);

    setPixelByStruct(xx,yy, normalizeAll(r,g,b));

}

void setPixelByStruct(int x, int y, Kolor kolor){
    setPixel(x,y,kolor.r,kolor.g,kolor.b);
}
void setYIQ(int xx, int yy, float y, float i, float q){
    float r = y + (0.956 * i) + (0.619 * q);
    float g = y + (-0.272 * i) + (-0.647 * q);
    float b = y + (-1.106 * i) + (1.703 * q);

 setPixelByStruct(xx,yy, normalizeAll(r,g,b));

 }

void setYCbCr(int xx, int yy, float y, float cb, float cr){
    float r = y + 1.402 * (cr - 128);
    float g = y - 0.344136 * (cb -128) - 0.714136 * (cr -128);
    float b = y + 1.772 * (cb -128);

    setPixelByStruct(xx,yy, normalizeAll(r,g,b));

}
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


HSL getHSL(int xx, int yy){
    SDL_Color bit8color  = getPixel(xx,yy);
    SDL_Color base = z6Kdo24K(z24Kdo6K(bit8color));

    float r = base.r/255.0;
    float g = base.g/255.0;
    float b = base.b/255.0;

    float maxRGB = max(max(r,g),b);
    float minRGB = min(min(r,g),b);
    float lum = (maxRGB + minRGB )/2;
    float sat;

    //cout<<maxRGB<<" , "<<minRGB<<endl;

    if(minRGB == maxRGB){
        sat = 0.0;
    }
    else if(lum<=0.5){
        sat = (maxRGB-minRGB)/ (maxRGB+minRGB);
    }
    else if(lum>0.5)
    {
    sat = (maxRGB-minRGB)/ (2.0 - maxRGB - minRGB);
    }

     float hue;

     if(r == maxRGB){
        hue = (g-b)/ (maxRGB-minRGB);
     }
     else if(g == maxRGB){
        hue = 2.0 + (b-r)/(maxRGB-minRGB);
     }
     else if(b == maxRGB){
        hue = 4.0 + (r-g)/(maxRGB-minRGB);
     }

     hue*=60.0;
     if(hue<0){
        hue+=360;
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
        var1 = l+s-l*s;
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

float normalizeHsl(float x){
    if(x>1.0){
        return x-1.0;
    }
    else if(x<0.0){
        return x+1.0;
    }
    return x;
}

float test(float color , float var1, float var2){
    float tempColor;
    if(6*color <1){
        tempColor = var2+(var1-var2)*6*color;
    }
    //test 2
    else if(2*color <1){
            tempColor = var1;
    }
    //test3
    else if(3*color<2){
            tempColor = var2 +(var1-var2) * (0.666- color)*6;
    }
    else if(3*color>2){
        tempColor = var2;
    }
    return tempColor;
}

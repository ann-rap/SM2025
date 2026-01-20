// główny plik projektu
#include "SM2025-Zmienne.h"
#include "SM2025-Funkcje.h"
#include "SM2025-Paleta.h"
#include "SM2025-MedianCut.h"
#include "SM2025-Pliki.h"
#include <thread>
#include <string>
#include <fstream>

#include <exception>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
using namespace std;

bool czyPlikIstnieje(string nazwa) {
    ifstream f(nazwa.c_str());
    return f.good();
}





void menu() {
    cout << "\n=== PROJEKT SM2025 ===\n";
    cout << "Wpisz 'q' aby wrocic do podgladu.\n";

    string in, out;
    char choice;
    int tryb = 1;
    int kompresja = 0;
    int predykcja = 0;

    SDL_Event e;
    while(SDL_PollEvent(&e)) {}

    while (true) {
        cout << "\n[MENU] Podaj plik wejsciowy: ";
        cin >> in;

        if (in == "q") {
            cout << "Powrot do okna graficznego.\n";
            return;
        }

        if (!czyPlikIstnieje(in)) {
            cout << "Plik nie istnieje!\n";
            continue;
        }

        bool isCustom = (in.length() >= 5 && in.substr(in.length() - 5) == ".dg24");

        if (isCustom) {
            wczytajDG24(in);
        } else {
            ladujBMP(in.c_str(), 0, 0);
        }

        SDL_UpdateWindowSurface(window);
        while(SDL_PollEvent(&e));

        if (isCustom) {
            cout << "Podaj plik wyjsciowy (BMP): ";
            cin >> out;
            if (out.length() < 4 || out.substr(out.length() - 4) != ".bmp") out += ".bmp";

            zapiszCwiartkeJakoBMP(out);
            cout << "Zapisano BMP.\n";
        }
        else {
            cout << "Podaj plik wyjsciowy (DG24): ";
            cin >> out;
            if (out.length() < 5 || out.substr(out.length() - 5) != ".dg24") out += ".dg24";

            cout << "Tryb 16-bit? (y/n): ";
            cin >> choice;

            if (choice == 'y') {
                cout << "Dithering? (y/n): ";
                cin >> choice;
                tryb = (choice == 'y') ? 3 : 0;

                cout << "Predykcja? (y/n): ";
                cin >> choice;
                predykcja = (choice == 'y') ? 1 : 0;

                cout << "Kompresja RLE? (y/n): ";
                cin >> choice;
                kompresja = (choice == 'y') ? 1 : 0;
            }
            else {
                cout << "RGB? (y - Tak | n - YCbCr): ";
                cin >> choice;
                bool isRGB = (choice == 'y');

                cout << "Skala szarosci? (y/n): ";
                cin >> choice;
                bool isGray = (choice == 'y');

                if (isRGB) {
                    tryb = isGray ? 4 : 1;

                    cout << "Kompresja RLE? (y/n): ";
                    cin >> choice;
                    kompresja = (choice == 'y') ? 1 : 0;

                    cout << "Predykcja? (y/n): ";
                    cin >> choice;
                    predykcja = (choice == 'y') ? 1 : 0;
                } else {
                    tryb = isGray ? 5 : 2;

                    int k;
                    cout << "Kompresja? (2-DCT | 1-RLE | 0-Brak): ";
                    cin >> k;
                    if (k != 1 && k != 2) k = 0;
                    kompresja = k;

                    if (kompresja != 2) {
                        cout << "Predykcja? (y/n): ";
                        cin >> choice;
                        predykcja = (choice == 'y') ? 1 : 0;
                    } else {
                        predykcja = 0;
                    }
                }
            }
            zapiszDG24(out, tryb, predykcja, kompresja);
        }
    }
}


int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
    }

    window = SDL_CreateWindow(tytul, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, szerokosc*2, wysokosc*2, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    screen = SDL_GetWindowSurface(window);
    if (screen == NULL) {
        fprintf(stderr, "SDL_GetWindowSurface Error: %s\n", SDL_GetError());
    return false;
    }
    SDL_UpdateWindowSurface(window);

    std::thread menuWatek(menu);

    // Detach pozwala wątkowi działać w tle, podczas gdy main idzie dalej
    menuWatek.detach();

    bool done = false;
    SDL_Event event;
    // główna pętla programu
    while (SDL_WaitEvent(&event)) {
        // sprawdzamy czy pojawiło się zdarzenie
        switch (event.type) {
            case SDL_QUIT:
                done = true;
                break;



            // sprawdzamy czy został wciśnięty klawisz
            case SDL_KEYDOWN: {
                // wychodzimy, gdy wciśnięto ESC
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    done = true;
                if (event.key.keysym.sym == SDLK_1)
                    Funkcja1();
                if (event.key.keysym.sym == SDLK_2)
                    Funkcja2();
                if (event.key.keysym.sym == SDLK_3)
                    Funkcja3();
                if (event.key.keysym.sym == SDLK_4)
                    Funkcja4();
                if (event.key.keysym.sym == SDLK_5)
                    Funkcja5();
                if (event.key.keysym.sym == SDLK_6)
                    Funkcja6();
                if (event.key.keysym.sym == SDLK_7)
                    Funkcja7();
                if (event.key.keysym.sym == SDLK_8)
                    Funkcja8();
                if (event.key.keysym.sym == SDLK_9)
                    Funkcja9();
                if (event.key.keysym.sym == SDLK_a)
                    ladujBMP("obrazek1.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_s)
                    ladujBMP("obrazek2.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_d)
                    ladujBMP("obrazek3.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_f)
                    ladujBMP("obrazek4.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_g)
                    ladujBMP("obrazek5.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_h)
                    ladujBMP("obrazek6.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_j)
                    ladujBMP("obrazek7.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_k)
                    ladujBMP("obrazek8.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_l)
                    ladujBMP("obrazek9.bmp", 0, 0);
                if (event.key.keysym.sym == SDLK_b)
                    czyscEkran(0, 0, 0);

                else
                    break;
               }
        }
        if (done) break;
    }

    if (screen) {
        SDL_FreeSurface(screen);
    }

    if (window) {
        SDL_DestroyWindow(window);
    }


    SDL_Quit();
    return 0;
}

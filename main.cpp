#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

#define MAX_ROZMIAR 100 ///ilosc znakow jaka bedzie wczytywana do pamieci

int utworzenie_tablicy_ascii(ifstream& plik, int tablica[MAX_ROZMIAR]) { ///tworzenie tablicy liczb na podstawie znakow zczytanych z plikow
    char znak;
    int ile=0;

    while( ile < MAX_ROZMIAR && plik.get(znak)) { ///konczy sie gdy dojdziemy do konca pliku albo osiagniemy maksywalna ilosc wczytanych znakow
        if(!isspace(znak)) { ///zamieniamy tylko wtedy gdy nie jest znakiem bialym jesli jest to pomijamy i przechodzimy do nastepnego
            znak = tolower(znak);
            tablica[ile] = static_cast<int>(znak);
            ile++;
        }
    }
    return ile;
}

float srednia_ciagu(ifstream& plik, int& suma_pliku, int& dlugosc_pliku) { ///liczenie wartosci sredniej ciagu liczb utworzonych na podstawie znakow
    int suma = 0, ile = 0;
    char znak;

    while(plik.get(znak)) {
        if(!isspace(znak)) {
            znak = tolower(znak); ///zamiana wszystkich liter na male
            suma += static_cast<int>(znak); ///dodanie do sumy liczby ktora odpowiada danej literze
            ile++;
        }
    }
    suma_pliku = suma;
    dlugosc_pliku = ile;
    return suma/ile;
}

float licz_licznik(int x[MAX_ROZMIAR], int y[MAX_ROZMIAR], float sr_x, float sr_y, int ile) { ///obliczenie wartosci licznika w rownaniu na wspolczynnik korelacji
    float suma=0;
    for(int i=0; i<ile; i++) {
        suma += (x[i]-sr_x) * (y[i]-sr_y);
    }
    return suma;
}

float licz_mianownik(int tablica[MAX_ROZMIAR], int srednia, int ile) { ///obliczenie wartosci mianownika w rownaniu na wspolczynnik korelacji
    float suma=0;
    for(int i=0; i<ile; i++) {
        suma += (tablica[i]-srednia)*(tablica[i]-srednia);
    }
    return suma;
}

float wspolczynnik_korelacji(float licznik, float mianownik_1, float mianownik_2) {
    float wspolczynnik;

    if (mianownik_1 == 0 || mianownik_2 == 0) wspolczynnik = 0;
    else wspolczynnik = licznik / (sqrt(mianownik_1)*sqrt(mianownik_2));

    return abs(wspolczynnik);
}

float porownanie_plikow(ifstream& plik_1, ifstream& plik_2) {
    char znak;
    int x[MAX_ROZMIAR], y[MAX_ROZMIAR], ile_znakow_x, ile_znakow_y;
    float wspolczynnik, sr_x, sr_y, licznik = 0, mianownik_1 = 0, mianownik_2 = 0;
    int dlugosc_plik_1, dlugosc_plik_2, suma_plik_1, suma_plik_2;

    sr_x = srednia_ciagu(plik_1, suma_plik_1, dlugosc_plik_1); ///srednia liczb pierwszego pliku
    sr_y = srednia_ciagu(plik_2, suma_plik_2, dlugosc_plik_2); ///srednia liczb drugiego pliku

    if(dlugosc_plik_1 > dlugosc_plik_2) {           ///jezeli jakis ciag jest dluzszy to srednia krotszego jest ponownie liczona
        sr_y = suma_plik_2/dlugosc_plik_1;          ///przez podzielenie sumy wszsytkich liczb w ciagu przez liczbe znakow w dluzszym ciagu
    } else if(dlugosc_plik_1 < dlugosc_plik_2){
        sr_x = suma_plik_1/dlugosc_plik_2;
    }

    plik_1.clear();             ///przejscie do poczatku pliku
    plik_1.seekg(0, ios::beg);
    plik_2.clear();             ///przejscie do poczatku pliku
    plik_2.seekg(0, ios::beg);
    do {
        ile_znakow_x = utworzenie_tablicy_ascii(plik_1, x); ///utworzenie tablicy ascii ze znakow w pliku
        ile_znakow_y = utworzenie_tablicy_ascii(plik_2, y);
        if(ile_znakow_x < ile_znakow_y) {                   ///jezeli jakis ciag jest dluzszy to krotszy jest zwiekszany przy pomocy zer
            for(int i=ile_znakow_x; i<ile_znakow_y; i++) x[i] = 0;
            ile_znakow_x = ile_znakow_y;
        } else {
            for(int i=ile_znakow_y; i<ile_znakow_x; i++) y[i] = 0;
            ile_znakow_y = ile_znakow_x;
        }
        licznik += licz_licznik(x, y, sr_x, sr_y, ile_znakow_x); ///obliczenie licznika
        mianownik_1 += licz_mianownik(x, sr_x, ile_znakow_x);   ///obliczenie pierwszej czesci mianownika
        mianownik_2 += licz_mianownik(y, sr_y, ile_znakow_y);   ///obliczenie drugiej czesci mianownika
    } while(ile_znakow_x == MAX_ROZMIAR); ///jezeli pilk jest dluzszy niz maksymalny rozmiar to wszytujemy kolejne czesci pliku

    wspolczynnik = wspolczynnik_korelacji(licznik, mianownik_1, mianownik_2);
    return wspolczynnik;
}

bool czy_istnieje(ifstream& plik) { ///sprawdzanie czy plik istnieje, jezeli tak to zwraca true a jezeli nie to prosi o ponowne podanie nazwy pliku
    char doc[MAX_ROZMIAR];
    if(plik.fail()) {
        plik.close();
        cout << "Plik nie istnieje!" << endl;
        cout << "Podaj jeszcze raz nazwe pliku: ";
        cin >> doc;
        plik.open(doc, ios::in);
        return false;
    } else {
        return true;
    }
}

int main()
{
    char doc_1[MAX_ROZMIAR], doc_2[MAX_ROZMIAR];
    float wspolczynnik;
    bool plik_istnieje;
    ifstream plik_1, plik_2;

    cout << "Program porownuje dwa pliki tekstowe i podaje stopien ich wzajemnego" << endl;
    cout << "podobienstwa na podstawie wspolczynnika korelacji Pearsona." << endl << endl;
    cout << "Podaj nazwe plikow (z rozszerzeniem), ktore chcesz porownac:" << endl;
    cout << "Pierwszy plik: ";
    cin >> doc_1;
    plik_1.open(doc_1, ios::in);
    plik_istnieje = czy_istnieje(plik_1);
    while(!plik_istnieje) {
        plik_istnieje = czy_istnieje(plik_1);
    }

    cout << "Drugi plik: ";
    cin >> doc_2;
    plik_2.open(doc_2, ios::in);
    plik_istnieje = czy_istnieje(plik_2);
    while(!plik_istnieje) {
        plik_istnieje = czy_istnieje(plik_2);
    }

    wspolczynnik = porownanie_plikow(plik_1, plik_2);

    cout << "Wspolczynnik korelacji Pearsona wynosi: " << wspolczynnik << endl;
    if(wspolczynnik > 0.5) {
        cout << "oznacza to, ze podane pliki sa podobne(plagiat)." << endl;
    } else {
        cout << "oznacza to, ze podane pliki nie sa podobne(nie sa plagiatem)." << endl;
    }

    plik_1.close();
    plik_2.close();
    return 0;
}

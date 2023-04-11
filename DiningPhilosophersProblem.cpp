#include<iostream>
#include<chrono>
#include<ncurses.h>
#include<mutex>
#include<thread>
#include<random>
#include<vector>
#include<string>
#include<stdarg.h>

using namespace std;

mutex widelce[15]; //tworzenie mutexów
mutex ncurses;// po to żeby ekran nie  był równocześnie modyfikowany
//getmaxyx(stdscr,10,2);
struct Filozof { //tworzenie struktury filozofa zawierającego jego atrybuty : imie, numer, informacje o stanie, i dwie liczby lewego i prawego widelca
    string imie;
    int numer;
    string corobi = "mysli";
    int lewy;
    int prawy;
    bool tab[15] = { false }; //tablica intow  0/1 (sprawdzanie czy odpowiedni mutex jest zajęty czy nie)

    Filozof(string imie_filozofa, int liczba, int l, int p) ://konstruktor , przypisujemy podczas tworzenia wartości do zmiennych 
        imie(imie_filozofa), numer(liczba), lewy(l), prawy(p)
    {
    }



    void myslenie() //funkcja stanu "myślenia" 
    {
        widelce_zajetewait();
        int czas = 1000;
        //int czas=rand()%(3000-2000)+2000;; //losowy czas wykonania myślenia 
        corobi = "mysli    "; //zmiana stanu na myślenie
        stan_filozofa(); //wypisanie na ekran stanu filozofa
        czas_dzialania(czas); //wypisywanie zmieniajacego sie czasu do konca myslenia
    }
    void jedzenie() //funkcja stanu "jedzenia"
    {
        int mniejszy, wiekszy;
        int czas = 5000;
        //int czas=rand()%(5000-2000)+2000;; //losowy czas wykonania jedzenia 
        if (lewy > prawy)//sprawdzanie ktory widelec jest mniejszy
        {
            mniejszy = prawy; //przypisanie widelca do zmniennej mniejszy/wiekszy
            wiekszy = lewy;
        }
        else
        {
            mniejszy = lewy;
            wiekszy = prawy;

        }
        corobi = "czeka    "; //zmieniamy stan na "czeka"
        stan_filozofa(); //wyswietlamy stan filozofa
        lock_guard<mutex> ga(widelce[mniejszy]);
        tab[mniejszy] = true;
        widelce_zajete();
        lock_guard<mutex> gb(widelce[wiekszy]);
        tab[wiekszy] = true;
        widelce_zajete();
        corobi = "konsumuje"; //jak oba są zajete to zmieniamy stan na "konsumuje"
        stan_filozofa(); //wyswietlamy stan filozofa
        czas_dzialania(czas); //wyswietlamy czas jedzenia
        widelce_zajete(); //wyswietlamy zajete widelce
    }
    void widelce_zajete() //wyswietlanie zajetych widelcow
    {
        string wywolanie = "Zajete widelce:"; //string 
        if (tab[lewy] == true)
        {
            wywolanie += to_string(lewy);
        }
        if (tab[prawy] == true)
        {
            wywolanie += to_string(prawy);
        }

        ncurses.lock(); //blokujemy mutex odpowiadajacy za wypisywanie na ekran
        attron(COLOR_PAIR(1)); //wybieramy palete kolorow 
        wmove(stdscr, numer, 40); //przemieszczamy sie do wybranego fragmentu ekranu
        wprintw(stdscr, (wywolanie.c_str())); //w wybranym fragmenice ekrany wypisujemy naszego stringa
        wclrtoeol(stdscr); //czyscimy wypisanie aby moc pozniej go nadpisac
        refresh(); //odswiezamy ekran
        attroff(COLOR_PAIR(1)); //konczymy palete kolorow
        ncurses.unlock(); //odblokowyjemy mutex odpowiadajacy za wypisywanie na ekran
    }
    void widelce_zajetewait() //wyswietlanie zajetych widelcow
    {
        string wywolanie = "Zajete widelce:"; //string 
        if (tab[lewy] == true)
        {
            wywolanie += to_string(lewy);
        }
        if (tab[prawy] == true)
        {
            wywolanie += to_string(prawy);
        }

        ncurses.lock(); //blokujemy mutex odpowiadajacy za wypisywanie na ekran
        attron(COLOR_PAIR(1)); //wybieramy palete kolorow 
        wmove(stdscr, numer, 40); //przemieszczamy sie do wybranego fragmentu ekranu
        wprintw(stdscr, (wywolanie.c_str())); //w wybranym fragmenice ekrany wypisujemy naszego stringa
        wclrtoeol(stdscr); //czyscimy wypisanie aby moc pozniej go nadpisac
        refresh(); //odswiezamy ekran
        attroff(COLOR_PAIR(1)); //konczymy palete kolorow
        ncurses.unlock(); //odblokowyjemy mutex odpowiadajacy za wypisywanie na ekran
    }
    void stan_filozofa() //wypisywanie na ekran stanu filozofa
    {
        string wywolanie = "Filozof " + to_string(numer) + " " + corobi; //sklejanie stringow 
        ncurses.lock(); //blokujemy mutex odpowiadajacy za wypisywanie na ekran
        attron(COLOR_PAIR(2)); //paleta kolorow
        wmove(stdscr, numer, 0); //przemieszczamy sie do wybranego fragmentu ekranu ( pierwsza kolumna, wiersz odpowiadajacy filozofowi)
        wprintw(stdscr, (wywolanie.c_str())); //wypisujemy na ekran naszego stringa
        //wclrtoeol(stdscr); //czyscimy wypisanie aby moc pozniej go nadpisac
        refresh(); //odswiezamy ekran
        attroff(COLOR_PAIR(2)); //konczymy paltete kolorow
        ncurses.unlock(); //odblokowyjemy mutex odpowiadajacy za wypisywanie
    }
    void czas_dzialania(int czas) //funkcja do wypisywania czasu
    {
        for (int i = czas; i > 0; i = i - 300) //dekrementujemy czas o pol sekundy zeby nie wyswietlao nam sie za szybko 
        {
            string wywolanie = to_string(i) + "ms";
            ncurses.lock();
            attron(COLOR_PAIR(3));
            wmove(stdscr, numer, 20);
            wprintw(stdscr, (wywolanie.c_str()));
            //wclrtoeol(stdscr);
            refresh();
            attroff(COLOR_PAIR(3));
            ncurses.unlock();
            this_thread::sleep_for(chrono::milliseconds(300)); //czekamy pol sekundy na zeby byl to rzeczywisty czas 
        }

    }
    void wykonywanie() //glowna petla dzialania filozofa ktora cały czas wywoluje najpierw myslenie a pozniej jedzenie
    {
        while (true)
        {
            myslenie();
            jedzenie();
            tab[lewy] = false;
            tab[prawy] = false;
        }
    }
};
int main(int argc, char* argv[]) //pobieranie argumentow z konsoli
{
    if (argc < 2) //jezeli jest mniej niz 2 to konczymy ( nazwa pliku i ilosc filozofow )
    {
        return 1;
    }
    int liczba_filozofow = stoi(argv[1]); //przypisujemy liczbe filozofow do zmienneij (argv[1] bo pierwszy czyli na miejscu 0 to nazwa pliku)
    if (liczba_filozofow < 5 || liczba_filozofow>10) //jezeli liczba filozofow jest <5 albo >10 konczymy petle
    {
        return 1;
    }


    //thread t;
    thread thread_filozof[liczba_filozofow];
    Filozof* filozofowie[liczba_filozofow];
    initscr(); //initujemy wyswietlanie ekranu
    cbreak(); //ignorowanie klikania na klawiaturze
    start_color(); //startowanie palety kolorow
    noecho(); //wylacza wypisywanie na ekan znakow wpisywanych przez uzytownika
    //palety kolorw (indeks, kolor tła, kolor czcionki)
    init_pair(1, COLOR_BLACK, COLOR_RED);
    init_pair(2, COLOR_BLACK, COLOR_GREEN);
    init_pair(3, COLOR_BLACK, COLOR_BLUE);
    init_pair(4, COLOR_BLACK, COLOR_YELLOW);
    //tworzenie nowych filozofow i ich wątkow
    for (int i = 0; i < liczba_filozofow; i++)
    {
        if (i == liczba_filozofow - 1) //jezeli natrafimy na ostatniego filozofa to przypisujemy mu jako lewy widelec widele nr 1 a jako prawy widelec nr ilosc filozofow 
        {
            filozofowie[i] = new Filozof("filozof" + to_string(i), i, 1, i + 1);
            thread_filozof[i] = thread(&Filozof::wykonywanie, filozofowie[i]);
        }
        else //jak to nie jest ostatni filozof to normalnie lewy widelec o jego numrzez a prawy o jeden wiekszy
        {
            filozofowie[i] = new Filozof("filozof" + to_string(i), i, i + 1, i + 2);

            thread_filozof[i] = thread(&Filozof::wykonywanie, filozofowie[i]);
        }
    }

    for (int i = 1; i <= liczba_filozofow; i++) //konczenie wątków filozofow
    {
        thread_filozof[i].join();
    }
    endwin(); //zamykanie okna
    return 0;


}
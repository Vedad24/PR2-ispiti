#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include <regex>
#include <fstream>
#include <cstring>
using namespace std;

const char* crt = "\n-------------------------------------------\n";
enum Karakteristike { NARUDZBA, KVALITET, PAKOVANJE, ISPORUKA };
ostream& operator<<(ostream& COUT, Karakteristike kar) {
    const char* karaker[] = { "NARUDZBA", "KVALITET", "PAKOVANJE", "ISPORUKA" };
    COUT << karaker[kar];
    return COUT;
}

bool ValidirajJedinstveniKod(string kod) {
    regex re("\\[[A-Z]{1,2}\\][0-9]{3,4}[-\\s]?\\{[a-z]{2}\\}");
    return regex_search(kod, re);
}

char* GetNizKaraktera(const char* tekst) {
    if (tekst == nullptr)
        return nullptr;
    int vel = strlen(tekst) + 1;
    char* temp = new char[vel];
    strcpy_s(temp, vel, tekst);
    return temp;
}

unique_ptr<char[]> getunique(const char* tekst) {
    if (tekst == nullptr)
        return nullptr;
    int vel = strlen(tekst) + 1;
    unique_ptr<char[]> temp = make_unique<char[]>(vel);
    strcpy_s(temp.get(), vel, tekst);
    return temp;
}

template<class T1, class T2, int max>
class Kolekcija {
    T1* _elementi1[max] = {nullptr};
    T2* _elementi2[max] = {nullptr};
    int* _trenutno = nullptr;
    bool _omoguciDupliranje;

public:
    Kolekcija(bool omoguciDupliranje = true) {
        _omoguciDupliranje = omoguciDupliranje;
        _trenutno = new int(0);
    }

    Kolekcija(const Kolekcija& obj) {
        _omoguciDupliranje = obj._omoguciDupliranje;
        for (int i = 0; i < *obj._trenutno; i++)
        {
            _elementi1[i] = new T1(*obj._elementi1[i]);
            _elementi2[i] = new T2(*obj._elementi2[i]);
        }
        _trenutno = new int(*obj._trenutno);
    }

    Kolekcija& operator=(const Kolekcija& obj) {
        if (this != &obj) {
            _omoguciDupliranje = obj._omoguciDupliranje;

            for (int i = 0; i < *_trenutno; i++)
            {
                delete _elementi1[i];
                delete _elementi2[i];
                _elementi1[i] = nullptr;
                _elementi2[i] = nullptr;
            }

            *_trenutno = *obj._trenutno;
            for (int i = 0; i < *_trenutno; i++)
            {
                _elementi1[i] = new T1(*obj._elementi1[i]);
                _elementi2[i] = new T2(*obj._elementi2[i]);
            }
        }
        return *this;
    }

    ~Kolekcija(){
        for (int i = 0; i < *_trenutno; i++)
        {
            delete _elementi1[i];
            delete _elementi2[i];
            _elementi1[i] = nullptr;
            _elementi2[i] = nullptr;
        }
    }

    void AddElement(T1 el1, T2 el2) {
        if (*_trenutno >= max)
            throw runtime_error("Nema prostora");
        if (!_omoguciDupliranje)
        {
            for (int i = 0; i < *_trenutno; i++)
            {
                if (*_elementi1[i] == el1 && *_elementi2[i] == el2)
                    throw runtime_error("Nije moguce duplirati!");
            }
        }
        _elementi1[*_trenutno] = new T1(el1);
        _elementi2[*_trenutno] = new T2(el2);
        (*_trenutno)++;
    }

    Kolekcija<T1,T2,max> RemoveAt(int index){
        if (index < 0 || index >= *_trenutno)
            throw runtime_error("Index nije validan");

        Kolekcija<T1, T2, max> temp;

        temp.AddElement(*_elementi1[index], *_elementi2[index]);

        for (int i = index; i < *_trenutno - 1 ; i++)
        {
            _elementi1[i] = _elementi1[i + 1];
            _elementi2[i] = _elementi2[i + 1];
        }

        (*_trenutno)--;
        return temp;
    }

    T1& getElement1(int lokacija)const { return *_elementi1[lokacija]; }
    T2& getElement2(int lokacija)const { return *_elementi2[lokacija]; }
    int getTrenutno() { return *_trenutno; }
    friend ostream& operator<< (ostream& COUT, const Kolekcija& obj) {
        for (size_t i = 0; i < *obj._trenutno; i++)
            COUT << obj.getElement1(i) << " " << obj.getElement2(i) << endl;
        return COUT;
    }
};
class Datum {
    int* _dan = nullptr, * _mjesec = nullptr, * _godina = nullptr;
public:
    Datum(int dan = 1, int mjesec = 1, int godina = 2000) {
        _dan = new int(dan);
        _mjesec = new int(mjesec);
        _godina = new int(godina);
    }

    Datum(const Datum& obj) {
        _dan = new int(*obj._dan);
        _mjesec = new int(*obj._mjesec);
        _godina = new int(*obj._godina);
    }

    Datum& operator=(const Datum& obj) {
        if (this != &obj) {
            *_dan = *obj._dan;
            *_mjesec = *obj._mjesec;
            *_godina = *obj._godina;
        }
        return *this;
    }

    ~Datum() {
        delete _dan; _dan = nullptr;
        delete _mjesec; _mjesec = nullptr;
        delete _godina; _godina = nullptr;
    }
    friend ostream& operator<< (ostream& COUT, const Datum& obj) {
        COUT << *obj._dan << "." << *obj._mjesec << "." << *obj._godina;
        return COUT;
    }
};
class ZadovoljstvoKupca {
    char* _komentar;
    //uz komentar, kupci mogu ocijeniti svaku od karakteristika proizvoda, a int se odnosi na ocjenu u rasponu od 1 - 10.
    //onemoguciti dupliranje karakteristika tj. svaki karakteristika se moze ocijeniti samo jednom...u suprotnom baciti objekat tipa exception
    Kolekcija<Karakteristike, int, 4>* _ocjeneKarakteristika;
public:
    ZadovoljstvoKupca(const char* komentar = "") {
        //kreirajte funkciju GetNizKaraktera ili na drugi adekvatan nacin izvrsite inicijalizaciju pokazivaca
        _komentar = GetNizKaraktera(komentar);
        _ocjeneKarakteristika = new Kolekcija<Karakteristike, int, 4>();
    }

    ZadovoljstvoKupca(const ZadovoljstvoKupca& obj) {
        _komentar = GetNizKaraktera(obj._komentar);
        _ocjeneKarakteristika = new Kolekcija<Karakteristike, int, 4>(*obj._ocjeneKarakteristika);
    }

    ZadovoljstvoKupca& operator=(const ZadovoljstvoKupca& obj) {
        if (this != &obj) {
            delete[] _komentar;
            _komentar = nullptr;
            _komentar = GetNizKaraktera(obj._komentar);
            delete _ocjeneKarakteristika;
            _ocjeneKarakteristika = nullptr;
            _ocjeneKarakteristika = new Kolekcija<Karakteristike, int, 4>(*obj._ocjeneKarakteristika);
        }
        return *this;
    }

    ~ZadovoljstvoKupca() {
        delete[] _komentar; _komentar = nullptr;
        delete _ocjeneKarakteristika; _ocjeneKarakteristika = nullptr;
    }

    void AddOcjenuKarakteristike(Karakteristike kar, int ocjena) {
        if (ocjena < 1 || ocjena > 10)
            return;
        for (int i = 0; i < _ocjeneKarakteristika->getTrenutno(); i++)
        {
            if (_ocjeneKarakteristika->getElement1(i) == kar)
                throw runtime_error("kakrakteristika je vec ocjenjena");
        }
        _ocjeneKarakteristika->AddElement(kar, ocjena);
    }

    float Prosjecna() {
        float rez = 0.00f;
        if (_ocjeneKarakteristika->getTrenutno() == 0)
            return rez;

        for (int i = 0; i < _ocjeneKarakteristika->getTrenutno(); i++)
        {
            rez += _ocjeneKarakteristika->getElement2(i);
        }

        return rez / _ocjeneKarakteristika->getTrenutno();
    }

    char* GeKomentar() { return _komentar; }
    Kolekcija<Karakteristike, int, 4>* GetOcjeneKarakteristika() { return _ocjeneKarakteristika; }

    friend ostream& operator<<(ostream& COUT, const ZadovoljstvoKupca& obj) {
        COUT << "Komentar: " << obj._komentar << endl;
        for (int i = 0; i < obj._ocjeneKarakteristika->getTrenutno(); i++)
        {
            COUT << "\t" << obj._ocjeneKarakteristika->getElement1(i)
            << "(" << obj._ocjeneKarakteristika->getElement2(i) << ")" << endl;
        }
        return COUT;
    }
};

class Kupac {
    unique_ptr<char[]>_imePrezime;
    string _emailAdresa;
    string _jedinstveniKod;
public:
    Kupac(const char* imePrezime, string emailAdresa, string jedinstveniKod) {
        _imePrezime = getunique(imePrezime);
        _emailAdresa = emailAdresa;
        _jedinstveniKod = (ValidirajJedinstveniKod(jedinstveniKod)) ? jedinstveniKod : "NOT VALID";
    }

    Kupac(const Kupac& obj) {
        _imePrezime = getunique(obj._imePrezime.get());
        _emailAdresa = obj._emailAdresa;
        _jedinstveniKod = obj._jedinstveniKod;
    }

    Kupac& operator=(const Kupac& obj) {
        if (this != &obj) {
            _imePrezime = getunique(obj._imePrezime.get());
            _emailAdresa = obj._emailAdresa;
            _jedinstveniKod = obj._jedinstveniKod;
        }
        return *this;
    }

    bool operator==(const Kupac& obj) {
        return (_emailAdresa == obj._emailAdresa || _jedinstveniKod == obj._jedinstveniKod);
    }


    string GetEmail() { return _emailAdresa; }
    string GetJedinstveniKod() { return _jedinstveniKod; }
    char* GetImePrezime() { return _imePrezime.get(); }

    friend ostream& operator<< (ostream& COUT, const Kupac& obj) {
        COUT << obj._imePrezime.get() << " " << obj._emailAdresa << " " << obj._jedinstveniKod << endl;
        return COUT;
    }
};

mutex mtx;

class Kupovina {
    Datum _datumKupovine;
    //jedna kupovina moze podrazumjevati isporuku proizvoda vecem broju kupaca
    //onemoguciti dodavanje kupaca sa istom email adresom ili jedinstvenim kodomu okviru jedne kupovine
    vector<Kupac> _kupaci;
    //za svaku kupovinu se moze ostaviti samo jedan komentar odnosno ocjena zadovoljstva, bez obzirana broj kupaca
    ZadovoljstvoKupca _zadovoljstvoKupca;

    void SaljiMail(ZadovoljstvoKupca obj) {
        mtx.lock();
        cout << "TO: " << endl;
        for (int i = 0; i < _kupaci.size(); i++)
        {
            cout << _kupaci[i].GetEmail() << endl;
        }
        cout << "Subject: Informacija" << endl
            << "Postovani,\nZaprimili smo Vase ocjene, a njihova\
         prosjecna vrijednost je: " << obj.Prosjecna() << endl;
        cout << "Zao nam je zbog toga, te ce Vas u najkracem periodu kontaktirati nasa Sluzba za odnose sa kupcima." << endl;
        cout << "Nadamo se da cete i dalje poslovati sa nama\nPuno pozdrava" << endl;
        mtx.unlock();
    }
public:
    Kupovina(Datum& datum, Kupac& kupac) : _datumKupovine(datum) {
        _kupaci.push_back(kupac);
    }

    Kupovina(const Kupovina& obj) {
        _datumKupovine = obj._datumKupovine;
        _kupaci = obj._kupaci;
        _zadovoljstvoKupca = obj._zadovoljstvoKupca;
    }

    Kupovina operator=(const Kupovina& obj) {
        if (this != &obj) {
            _datumKupovine = obj._datumKupovine;
            _kupaci = obj._kupaci;
            _zadovoljstvoKupca = obj._zadovoljstvoKupca;
        }
        return *this;
    }

    bool AddKupca(Kupac kupac) {
        for (int i = 0; i < _kupaci.size(); i++)
        {
            if (_kupaci[i] == kupac)
                return false;
        }
        _kupaci.push_back(kupac);
        return true;
    }

    void SetZadovoljstvoKupca(ZadovoljstvoKupca zad) {
        _zadovoljstvoKupca = zad;
        int counter = 0;
        for (int i = 0; i < _zadovoljstvoKupca.GetOcjeneKarakteristika()->getTrenutno(); i++)
        {
            if (_zadovoljstvoKupca.GetOcjeneKarakteristika()->getElement2(i) < 5)
                counter++;
        }
        if (counter >= 2)
        {
            thread t1(&Kupovina::SaljiMail, this, zad);
            t1.join();
        }
    }

    tuple<int, int, int> GetBrojZnakova(string tekstFile) {

        int velikaS = 0, malaS = 0, razmaci = 0;
        char znak;

        ifstream fajl(tekstFile);

        if (!fajl.fail()) {
            while (fajl.get(znak))
            {
                if (znak >= 'A' && znak <= 'Z')
                    velikaS++;
                if (znak >= 'a' && znak <= 'z')
                    malaS++;
                if (znak == ' ')
                    razmaci++;
            }
        }

        fajl.close();
        return { velikaS,malaS,razmaci };
    }

    vector<Kupac>& GetKupci() { return _kupaci; }
    ZadovoljstvoKupca GetZadovoljstvoKupca() { return _zadovoljstvoKupca; }
    friend ostream& operator<< (ostream& COUT, Kupovina& obj) {
        COUT << crt << "Kupovina " << obj._datumKupovine << " za kupce: " << endl;
        for (size_t i = 0; i < obj._kupaci.size(); i++)
        {
            COUT << "\t" << i + 1 << "." << obj._kupaci[i];
        }
        cout << crt << "Zadovoljstvo kupca: " << endl << obj._zadovoljstvoKupca << crt;
        return COUT;
    }

};
////////const char* GetOdgovorNaPrvoPitanje() {
////////    cout << "Pitanje -> \Nabrojite i ukratko pojasnite osnovne ios modove koji se koriste u radu sa fajlovima?\n";
////////    return "Odgovor -> OVDJE UNESITE VAS ODGOVOR";
////////}
////////const char* GetOdgovorNaDrugoPitanje() {
////////    cout << "Pitanje -> Pojasnite prednosti i nedostatke visestrukog nasljedjivaja, te ulogu virtualnog nasljedjivanja u tom kontekstu?\n";
////////    return "Odgovor -> OVDJE UNESITE VAS ODGOVOR";
////////}

int main() {

    /*cout << PORUKA;
    cin.get();

    cout << GetOdgovorNaPrvoPitanje() << endl;
    cin.get();
    cout << GetOdgovorNaDrugoPitanje() << endl;
    cin.get();*/

    Datum datum30062022(30, 6, 2022);

    const int KolekcijaTestSize = 9;
    Kolekcija<int, int, KolekcijaTestSize> Kolekcija1(false);

    for (int i = 0; i < KolekcijaTestSize - 1; i++)
        Kolekcija1.AddElement(i, i);//dodaje vrijednosti u Kolekcija

    try {
        //ukoliko nije dozvoljeno dupliranje elemenata (provjeravaju se T1 i T2), metoda AddElement treba baciti objekat tipa exception
        Kolekcija1.AddElement(3, 3);
    }
    catch (exception& err) {
        cout << err.what() << crt;
    }
    Kolekcija1.AddElement(9, 9);

    try
    {
        //ukoliko je Kolekcija popunje, metoda AddElement treba baciti objekat tipa exception
        Kolekcija1.AddElement(10, 10);
    }
    catch (exception& err) {
        cout << err.what() << crt;
    }

    cout << Kolekcija1 << crt;

    /* metoda RemoveAt treba da ukloni i vrati (kao povratnu vrijednost)clanoveKolekcijaa koji se nalaze na lokaciji definisanoj prvim parametrom.
    * Nakon izvrsenja metode RemoveAt, objekat Kolekcija1 bi trebao posjedovati sljedeci sadrzaj:
    * 0  0
    * 1  1
    * 3  3
    * 4  4
    * ....
    *
    * a objekat Kolekcija2 samo jedan par:
    *  2  2
    */
    Kolekcija<int, int, KolekcijaTestSize> Kolekcija2 = Kolekcija1.RemoveAt(2);

    cout << Kolekcija2 << crt;

    cout << Kolekcija1 << crt;


    /*
    * za validaciju broja pasosa koristiti funkciju ValidirajBrojPasosa koja treba, koristeci regex, osigurati postivanje
    sljedeceg formata:
    - pocetak sadrzi jedan ili dva velika slova unutar uglastih zagrada
    - nakon toga slijede tri ili cetiri broja
    - nakon toga moze, a ne mora, doci znak crtica (-) ili razmak ( )
    - nakon toga slijede dva mala slova unutar viticastih zagrada

    pomenutu funkciju iskoristiti u konstruktoru klase Kupac, te ukoliko jedinstveni kod  nije u validnom formatu onda njegova vrijednost treba biti postavljena na NOT VALID
    */

    // \[ [A - Z]{1, 2} \] [0 - 9]{ 3,4 } [-\s] ? \{[a - z] {2}\}
     
    if (ValidirajJedinstveniKod("[BH]235-{mo}"))
        cout << "Jedinstveni kod validan" << endl;
    if (ValidirajJedinstveniKod("[MO]235{sa}"))
        cout << "Jedinstveni kod validan" << endl;
    if (ValidirajJedinstveniKod("[B]2356 {ih}"))
        cout << "Jedinstveni kod validan" << endl;
    if (!ValidirajJedinstveniKod("[s]2356{ih}"))
        cout << "Jedinstveni kod NIJE validan" << endl;
    if (!ValidirajJedinstveniKod("[BH]2356{h}"))
        cout << "Jedinstveni kod NIJE validan" << endl;
    if (!ValidirajJedinstveniKod("BH2356{ih}"))
        cout << "Jedinstveni kod NIJE validan" << endl;


    Kupac denis("Denis Music", "denis@fit.ba", "[BH]235-{mo}");
    Kupac jasmin("Jasmin Azemovic", "jasmin@fit.ba", "[MO]235{sa}");
    Kupac adel("Adel Handzic", "adel@edu.fit.ba", "[B]2356 {ih}");
    Kupac jedinstveniKodNotValid("Ime Prezime", "korisnik@klix.ba", "BH2356{ih}");//_jedinstveniKod = NOT VALID


    /*cout << denis << endl;
    cout << jasmin << endl;
    cout << adel << endl;
    cout << jedinstveniKodNotValid << endl;*/

    //kupovini  se prilikom kreiranja mora dodijeliti kupac koji je kreator kupovine
    Kupovina kupovina(datum30062022, denis);
    if (kupovina.AddKupca(jasmin))
        cout << "Kupac uspjesno dodan!";

    cout << kupovina;

    /*cout << kupovina << endl;*/

    ZadovoljstvoKupca zadovoljstvoKupca("Nismo pretjerano zadovoljni kvalitetom, a ni pakovanjem");
    zadovoljstvoKupca.AddOcjenuKarakteristike(NARUDZBA, 7);
    zadovoljstvoKupca.AddOcjenuKarakteristike(KVALITET, 4);

    try {
        zadovoljstvoKupca.AddOcjenuKarakteristike(KVALITET, 4); //kakrakteristika je vec ocjenjena
    }
    catch (exception& err) {
        cout << err.what() << crt;
    }

    zadovoljstvoKupca.AddOcjenuKarakteristike(PAKOVANJE, 3);
    zadovoljstvoKupca.AddOcjenuKarakteristike(ISPORUKA, 6);

    /*cout << zadovoljstvoKupca << endl;*/

    //u okviru kupovine postavlja vrijednost atributa _zadovoljstvoKupca
    kupovina.SetZadovoljstvoKupca(zadovoljstvoKupca);

    //ukoliko se kupovini  doda zadovoljstvo koji sadrzi najmanje dvije karakteristike sa ocjenom manjom od 5, svim kupcima, 
    //koji su dodati u kupovinu, u zasebnom thread-u, se salje email sa sljedecim sadrzajem:
   /*
       To: denis@fit.ba;jasmin@fit.ba;
       Subject: Informacija

       Postovani,

       Zaprimili smo Vase ocjene, a njihova prosjecna vrijednost je X.X.
       Zao nam je zbog toga, te ce Vas u najkracem periodu kontaktirati nasa Sluzba za odnose sa kupcima.

       Nadamo se da cete i dalje poslovati sa nama
       Puno pozdrava
   */

   //ispisuje sve podatke o kupovini, kupcima, komentar, ocjene pojedinih karakteristika i prosjecnu ocjenu.
    cout << kupovina << endl;
    /* Primjer ispisa:
       -------------------------------------------
       Kupovina 30.6.2022 za kupce:
               1.Denis Music denis@fit.ba [BH]235-{mo}
               2.Jasmin Azemovic jasmin@fit.ba [MO]235{sa}
       -------------------------------------------
       Zadovoljstvo kupca:
       Nismo pretjerano zadovoljni kvalitetom, a ni pakovanjem.
             NARUDZBA(7)
             KVALITET(4)
             PAKOVANJE(3)
             ISPORUKA(6)
       -------------------------------------------
       Prosjecna ocjena -> 5
   */

    Kupovina kupovinaSaAdelom = kupovina;
    if (kupovinaSaAdelom.AddKupca(adel))
        cout << "Kupac uspjesno dodan!";
    if (!kupovinaSaAdelom.AddKupca(denis))
        cout << "Kupac je vec dodan!";

    cout << kupovinaSaAdelom << endl;

    /*metoda GetBrojZnakova treba prebrojati koliko se unutar fajla, cije ime je proslijedjeno kao parametar, nalazi: velikih slova, malih slova i razmaka..*/
   tuple<int, int, int> brojac = kupovinaSaAdelom.GetBrojZnakova("denis.txt");
    cout << "Velikih slova: " << get<0>(brojac) << endl;
    cout << "Malih slova: " << get<1>(brojac) << endl;
    cout << "Razmaka: " << get<2>(brojac) << endl;
    
    /*ukolikoo je sadrzaj fajla npr. "Denis Music" povratne vrijednosti metode GetBrojZnakova bi trebale biti 2 8 1. za potrebe testiranja kreirajte fajl sa zeljenim sadzajem
    */

   //////// cin.get();
   //////// system("pause>0");
}

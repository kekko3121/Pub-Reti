#include <vector>
#include "Tavolo.h"
using namespace std;

class Pub {
    private:

        int maxClienti;
        int maxClientiTavolo;
        int maxTavoli;
        vector<Tavolo> tavoli;

    public:

        Pub(int maxClienti, int maxClientiTavolo, int maxTavoli);
        void aggiungiTavolo(int maxSedie);
        bool postoDisponibile();
        bool aggiungiCliente();
        void preparaOrdine();
};

Pub::Pub(int maxClienti, int maxClientiTavolo, int maxTavoli) {
    this->maxClienti = maxClienti;
    this->maxClientiTavolo = maxClientiTavolo;
    this->maxTavoli = maxTavoli;
}

void Pub::aggiungiTavolo(int maxSedie) {
    if (tavoli.size() < maxTavoli) {
        tavoli.push_back(Tavolo(maxSedie, tavoli.size() + 1));
    }
}

bool Pub::postoDisponibile() {
    for (Tavolo& tavolo : tavoli) {
        if (tavolo.postoDisponibile()) {
            return true;
        }
    }
    return false;
}

bool Pub::aggiungiCliente() {
    if (postoDisponibile()) {
        for (Tavolo& tavolo : tavoli) {
            if (tavolo.addCliente()) {
                return true;
            }
        }
    }
    return false;
}

void Pub::preparaOrdine() {
    // Implementazione per preparare l'ordine
}
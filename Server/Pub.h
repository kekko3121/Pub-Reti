/*
 Classe Pub per rappresentare il Pub
*/

#ifndef PUB_H
#define PUB_H

#include <vector>
#include <string>
#include "Tavolo.h"
using namespace std;

class Pub {
    private:

        int maxClienti; // numero massimo di cliente che possono entrare nel locale
        int maxTavoli; // numero massimo di tavoli disponibili
        vector<Tavolo> tavoli; // lista dei tavoli nel locale

    public:
        //Creo un Pub con il numero massimo di clienti e di tavoli
        Pub(int maxClienti, int maxTavoli);
        bool aggiungiTavolo(int maxSedie); // metodo per aggiungere un tavolo nel locale
        bool postoDisponibile(int numeroTavolo); // metodo per verificare se ci sono posti nel tavolo scelto 
        bool aggiungiCliente(int numeroTavolo); // metodo per aggiungere il cliente al tavolo scelto
        string preparaOrdine(); // metodo per preparare l'ordine del cliente
        int tavoloVuoto(); //verifica se il tavolo è vuoto
        bool liberaPosto(int numeroTavolo); // metodo per liberare un posto al tavolo
        int postiDisponibili();
};

Pub::Pub(int maxClienti, int maxTavoli) {
    this->maxClienti = maxClienti; //imposto il numero massimo di clienti
    this->maxTavoli = maxTavoli; // imposto il numero massimo di tavoli
}

bool Pub::aggiungiTavolo(int maxSedie) {
    if (tavoli.size() < maxTavoli) { //Se non ho raggiunto il massimo di tavoli disponibili
        tavoli.push_back(Tavolo(maxSedie, tavoli.size() + 1)); //aggiungo un nuovo tavolo
        return true;
    }

    return false;
}

bool Pub::postoDisponibile(int numeroTavolo) {
    for (Tavolo& tavolo : tavoli) { //Scorro i tavoli
        if (tavolo.getNumeroTavolo() == numeroTavolo && tavolo.postoDisponibile()) { //In base al numero di tavolo verifico se ho un posto disponibile
            return true; // restituisce vero se c'è un postro
        }
    }
    return false; // restituisce falso se non c'è un posto
}

bool Pub::aggiungiCliente(int numeroTavolo) {
    for (Tavolo& tavolo : tavoli) {
        if (tavolo.getNumeroTavolo() == numeroTavolo && tavolo.addCliente()) { //In base al numero di tavolo aggiungo il cliente
            return true; // restituisce vero se è andato a buon fine la procedura
        }
    }
    return false; // altrimenti restituisce falso per un errore
}

int Pub::tavoloVuoto(){
    for(Tavolo& tavolo: tavoli){
        if(tavolo.tavoloVuoto()){ //Se il tavolo è vuoto
            return tavolo.getNumeroTavolo(); // restituisce il numero di tavolo
        }
    }

    return -1;
}

bool Pub::liberaPosto(int numeroTavolo) {
    for (Tavolo& tavolo : tavoli) {
        if (tavolo.getNumeroTavolo() == numeroTavolo) {
            tavolo.liberaPosto();
            return true;
        }
    }
    return false;
}

string Pub::preparaOrdine() {
    return "Sto preparando l'ordine";
}

int Pub::postiDisponibili() {
    int posti = 0;
    for (Tavolo& tavolo : tavoli) {
        posti += (tavolo.getMaxSedieTavolo() - tavolo.getNumeroClienti());
    }
    return posti;
}

#endif // PUB_H
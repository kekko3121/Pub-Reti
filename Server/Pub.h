/*
 Classe Pub per rappresentare il Pub
*/

#ifndef PUB_H
#define PUB_H

#include <iostream>
#include <map>
#include "Tavolo.h"
using namespace std;

class Pub {
    private:

        int maxClienti; // numero massimo di cliente che possono entrare nel locale
        int maxTavoli; // numero massimo di tavoli disponibili
        map<int, Tavolo> tavoli; // lista dei tavoli nel locale

    public:

        //Creo un Pub con il numero massimo di clienti e di tavoli
        Pub(int maxClienti, int maxTavoli);
        bool aggiungiTavolo(int maxSedie); // metodo per aggiungere un tavolo nel locale
        bool postoDisponibile(int numeroTavolo); // metodo per verificare se ci sono posti nel tavolo scelto 
        bool aggiungiCliente(int numeroTavolo); // metodo per aggiungere il cliente al tavolo scelto
        void preparaOrdine(int tavolo); // metodo per preparare l'ordine del cliente
        int tavoloVuoto(); //verifica se il tavolo è vuoto
        void liberaPosto(int numeroTavolo); // metodo per liberare un posto al tavolo
        int postiDisponibili(); // verifica se ci sono posti liberi nel Pub
};

//Costruttore per inizializzare l'array di tavoli e impostare il numero massimo di clienti e il numero massimo di tavoli
Pub::Pub(int maxClienti, int maxTavoli) : maxClienti(maxClienti), maxTavoli(maxTavoli) {}

bool Pub::aggiungiTavolo(int maxSedie) {
    if (tavoli.size() < maxTavoli) { //Se non ho raggiunto il massimo di tavoli disponibili
        tavoli.insert(pair<int, Tavolo>(tavoli.size() + 1, Tavolo(maxSedie))); //aggiungo un nuovo tavolo
        //incremento il numero di tavoli
        return true;
    }

    return false;
}

bool Pub::aggiungiCliente(int numeroTavolo) {
    if (tavoli.at(numeroTavolo).addCliente()) { //In base al numero di tavolo aggiungo il cliente
        return true; // restituisce vero se è andato a buon fine la procedura
    }
    return false; // altrimenti restituisce falso per un errore
}

int Pub::tavoloVuoto(){
    for(auto &x : tavoli){
        if(x.second.tavoloVuoto()){ //Se il tavolo è vuoto
            return x.first; // restituisce il numero di tavolo
        }
    }

    return -1;
}

void Pub::liberaPosto(int numeroTavolo) {
        tavoli.at(numeroTavolo).liberaPosto(); //libera il posto al numero del tavolo
}

void Pub::preparaOrdine(int ntavolo) {
    cout << "Sto preparando l'ordine del tavolo " << ntavolo << endl; //stampa per simulare l'ordine di preparazione
    sleep (5); //finta attesa
    cout << "Ordine pronto per essere consegnato al tavolo " << ntavolo << " dal cameriere" << endl; //stampa per simulare l'ordine pronto
}

int Pub::postiDisponibili() {
    int posti = 0; //variabile per contare il numero di posti disponibili
    for (auto &x : tavoli) {
        //calcola il numero di posti liberi in base al numero massimo di sedie disponibili
        posti += (x.second.getMaxSedieTavolo() - x.second.getNumeroClienti());
    }
    return posti; // ritorna il numero di posti disponibili
}

#endif // PUB_H
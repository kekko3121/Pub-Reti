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
        int ntavoli; // numero di tavolo presenti nel Pub
        int shmID; // identificatore memoria condivisa
        Tavolo* tavoli; // lista dei tavoli nel locale

    public:

        // Creo un Pub con il numero massimo di clienti e di tavoli
        Pub(int maxClienti, int maxTavoli);
        ~Pub(); // Distruttore per il dettach della memoria condivisa
        bool aggiungiTavolo(int maxSedie); // metodo per aggiungere un tavolo nel locale
        bool postoDisponibile(int numeroTavolo); // metodo per verificare se ci sono posti nel tavolo scelto 
        bool aggiungiCliente(int numeroTavolo); // metodo per aggiungere il cliente al tavolo scelto
        void preparaOrdine(int tavolo); // metodo per preparare l'ordine del cliente
        int tavoloVuoto(); //verifica se il tavolo è vuoto
        void liberaPosto(int numeroTavolo); // metodo per liberare un posto al tavolo
        int postiDisponibili(); // verifica se ci sono posti liberi nel Pub
};

/* Costruttore per inizializzare l'array di tavoli in memoria condivisa e impostare il numero massimo di clienti, il numero massimo di tavoli
   e inizizializzare ntavoli */
Pub::Pub(int maxClienti, int maxTavoli) : maxClienti(maxClienti), maxTavoli(maxTavoli), ntavoli(1) {
    
    // Creazione/accesso alla memoria condivisa per l'array tavoli
    if ((this->shmID = shmget(IPC_PRIVATE, sizeof(Tavolo), IPC_CREAT | 0666)) < 0) {
        cerr << "Errore durante la shmget per Tavolo" << endl;
        exit(1);
    }
    
    // Attacco della memoria condivisa
    this->tavoli = (Tavolo *) shmat(shmID, NULL, 0);
    if (tavoli == (Tavolo*)-1) {
        cerr << "Errore durante la shmat per tavoli" << endl;
        exit(1);
    }
}

Pub::~Pub(){

    // Dettach della memoria condivisa
    shmdt(tavoli);
    shmctl(shmID, IPC_RMID, NULL);
}

bool Pub::aggiungiTavolo(int maxSedie) {
    if (ntavoli <= maxTavoli) { //Se non ho raggiunto il massimo di tavoli disponibili
        new (&tavoli[ntavoli]) Tavolo(maxSedie); //aggiungo un nuovo tavolo
        
        //incremento il numero di tavoli
        ntavoli++;

        return true; // Restituisce true se il tavolo è stato aggiunto
    }

    return false; // Restituisce false se il tavolo non è stato aggiunto
}

bool Pub::aggiungiCliente(int numeroTavolo) {
    if (tavoli[numeroTavolo].addCliente()) { //In base al numero di tavolo aggiungo il cliente
        return true; // restituisce vero se è andato a buon fine la procedura
    }
    return false; // altrimenti restituisce falso per un errore
}

int Pub::tavoloVuoto(){
    for(int i = 1; i <= ntavoli; i++){ // Scorro tutti i tavoli
        if(tavoli[i].tavoloVuoto()){ //Se il tavolo è vuoto
            return i; // restituisce il numero di tavolo
        }
    }

    return -1; // altrimenti restituisce un numero negativo per segnalare che non ci sono tavoli vuoti 
}

void Pub::liberaPosto(int numeroTavolo) {
        tavoli[numeroTavolo].liberaPosto(); //libera il posto al numero del tavolo
}

void Pub::preparaOrdine(int ntavolo) {
    cout << "Sto preparando l'ordine del tavolo " << ntavolo << endl; //stampa per simulare l'ordine di preparazione
    sleep (5); //finta attesa
    cout << "Ordine pronto per essere consegnato al tavolo " << ntavolo << " dal cameriere" << endl; //stampa per simulare l'ordine pronto
}

int Pub::postiDisponibili() {
    int posti = 0; //variabile per contare il numero di posti disponibili
    for (int i = 1; i <= ntavoli; i++) { // Scorre tutti i tavoli
        //calcola il numero di posti liberi in base al numero massimo di sedie disponibili
        posti += (tavoli[i].getMaxSedieTavolo() - tavoli[i].getNumeroClienti());
    }
    return posti; // ritorna il numero di posti disponibili
}

#endif // PUB_H
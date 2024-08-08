/*
 Classe Tavolo per rappresentare i tavoli all'interno del Pub
*/

#ifndef TAVOLO_H
#define TAVOLO_H

class Tavolo{
    private:
        
        int maxSedieTavolo; // numero massimo di sedie disponibili per tavolo
        int* clientiSeduti; // numero di clienti seduti al tavolo

    public:

        //Crea un nuovo tavolo con numero di sedie disponibili
        Tavolo(int maxSedie, int* clientiSeduti);
        bool addCliente(); // aggiunge un cliente al tavolo
        bool postoDisponibile(); // verifica se il tavolo non è pieno
        void liberaPosto(); //libera il posto al tavolo
        int getNumeroTavolo(); // restituisce il numero del tavolo
        int getNumeroClienti(); //restituisce il numero di clienti seduti al tavolo
        int getMaxSedieTavolo(); //restituisce massimo di posti disponibile al tavolo
        bool tavoloVuoto(); // verifica se il tavolo è vuoto
};

// Costruttore per inizializzare clientiSeduti e impostare il numero massimo di sedie
Tavolo::Tavolo(int maxSedie, int* clientiSeduti) : maxSedieTavolo(maxSedie), clientiSeduti(clientiSeduti) {
        *clientiSeduti = 0;  // Inizializza il contatore nella memoria condivisa
}

bool Tavolo::addCliente(){
    if(postoDisponibile()){ //Se ci sono posti disponibili
        (*clientiSeduti)++; //aggiungo il cliente incrementando il contatore
        return true; // restituisce vero se ci sono posti disponibili
    }

    return false; // restituisce falso se non ci sono posti disponibili
}

bool Tavolo::postoDisponibile(){
    return *clientiSeduti < maxSedieTavolo; //restituisce true se il tavolo non è pieno altrimenti restituisce false
}

void Tavolo::liberaPosto(){
    if(*clientiSeduti > 0){
        (*clientiSeduti)--; // rimuove il cliente decrementando il contatore
    }
}

int Tavolo::getNumeroClienti(){
    return *clientiSeduti; // restituisce il numero di clienti seduti al tavolo
}

bool Tavolo::tavoloVuoto(){
    return *clientiSeduti == 0; // restituisce true se il tavolo è vuoto, altrimenti false
}

int Tavolo::getMaxSedieTavolo(){  //restituisce massimo di posti disponibile al tavolo
    return maxSedieTavolo;
}

#endif // TAVOLO_H
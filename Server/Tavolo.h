/*
 Classe Tavolo per rappresentare i tavoli all'interno del Pub
*/
class Tavolo{
    private:
        
        int maxSedieTavolo; // numero massimo di sedie disponibili per tavolo
        int clientiSeduti = 0; // numero di clienti seduti al tavolo
        int numeroTavolo; // numero del tavolo

    public:

        //Crea un nuovo tavolo con numero di sedie disponibili e numero del tavolo
        Tavolo(int maxSedie, int numeroTavolo);
        bool addCliente(); // aggiunge un cliente al tavolo
        bool postoDisponibile(); // verifica se il tavolo non è pieno
        void liberaPosto(); //libera il posto al tavolo
        int getNumeroTavolo(); // restituisce il numero del tavolo
        int getNumeroClienti(); //restituisce il numero di clienti seduti al tavolo
        int getMaxSedieTavolo(); //restituisce massimo di posti disponibile al tavolo
        bool tavoloVuoto(); // verifica se il tavolo è vuoto
};

Tavolo::Tavolo(int maxSedie, int numeroTavolo){
    this->maxSedieTavolo = maxSedie; //imposto il numero massimo di sedie
    this->numeroTavolo = numeroTavolo; //imposto il numero del tavolo
}

bool Tavolo::addCliente(){
    if(postoDisponibile()){ //Se ci sono posti disponibili
        clientiSeduti++; //aggiungo il cliente incrementando il contatore
        return true; // restituisce vero se ci sono posti disponibili
    }

    return false; // restituisce falso se non ci sono posti disponibili
}

bool Tavolo::postoDisponibile(){
    return clientiSeduti < maxSedieTavolo; //restituisce true se il tavolo non è pieno altrimenti restituisce false
}

void Tavolo::liberaPosto(){
    clientiSeduti--; // rimuove il cliente decrementando il contatore
}

int Tavolo::getNumeroTavolo(){
    return this->numeroTavolo; // restituisce il numero del tavolo
}

int Tavolo::getNumeroClienti(){
    return this->clientiSeduti; // restituisce il numero di clienti seduti al tavolo
}

bool Tavolo::tavoloVuoto(){
    return this->clientiSeduti == 0; // restituisce true se il tavolo è vuoto, altrimenti false
}
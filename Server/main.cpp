#include <iostream>
#include <string>
#include <signal.h>
#include "Pub.h"
#include "../Socket/Socket.h"
using namespace std;

// Socket globale per il signal handling
Socket* serverSocketPtr = nullptr;

// Signal handler per l'interruzione del programma
void signalHandler(int);

int main() {
    Socket serverSocket; //dichiarazione della socket server
    int numerotavolo;

    // Inizializzazione del pub con 5 tavoli e un massimo di 20 posti
    Pub pub(20, 5);

    //aggiunta dei tavoli
    pub.aggiungiTavolo(4);
    pub.aggiungiTavolo(6);
    pub.aggiungiTavolo(2);
    pub.aggiungiTavolo(5);
    pub.aggiungiTavolo(2);

    // Registrazione del signal handler
    signal(SIGINT, signalHandler);
    serverSocketPtr = &serverSocket;

    // Creazione della socket server
    if (!serverSocket.create()) {
        cerr << "Errore nella creazione del socket!" << endl;
        return -1;
    }

    // Costruisce la socket con la porta 25564
    if (!serverSocket.bind(25564)) {
        cerr << "Errore nell'associazione del socket alla porta!" << endl;
        return -1;
    }

    // Imposta la socket in ascolto
    if (!serverSocket.listen()) {
        cerr << "Errore nel mettere il socket in ascolto!" << endl;
        return -1;
    }

    cout << "Il pub è aperto in attesa di clienti" << endl;

    while (true) { // ciclo per accettare più richieste client

        Socket clientSocket; //socket per acquisire le informazioni del client

        if (!serverSocket.accept(clientSocket)) { // accetta la connessione del client
            cerr << "Errore nell'accettare la connessione!" << endl;
            return -1;
        }

        cout << "Cameriere ha preso servizio nel Pub" << endl; // stampa un messaggio di successo della connessione con il client

        string message; // memorizzo i messaggi che arrivano dal client

        if (clientSocket.receive(message) <= 0) { // se il messaggio non viene ricevuto correttamente
            break; // Connessione chiusa o errore ed esco dal ciclo
        }

        //verifica se ci sono posti disponibili per far accomodare il cliente
        if(message.compare("Ci sono posti liberi?") == 0){
            if(pub.postiDisponibili() > 0){
                if (!clientSocket.send("Si")) {
                    break; // Errore nell'invio
                }

                message.clear();

                clientSocket.receive(message);

                //Se il cliente decide di accomodarsi ad un nuovo tavolo, si controlla se ci sono tavoli vuoti disponibili
                if(message.compare("nuovo") == 0 && pub.tavoloVuoto() > 0){
                    clientSocket.send(to_string(pub.tavoloVuoto())); //Invia il numero di tavolo al cliente
                    pub.aggiungiCliente(pub.tavoloVuoto()); //aggiunge il cliente al tavolo
                }
                else if(message.compare("nuovo") == 0 && pub.tavoloVuoto() <= 0){ //Se non ci sono tavoli vuoti
                    clientSocket.send("Non ci sono tavoli vuoti"); //avvisa il cameriere
                }
                //Se il cliente decide di accomodarsi in un tavolo già occupato ma con posti disponibili
                else{
                    try {
                        int ntavolo = stoi(message); //conversione della stinga per recuperare il numero di tavolo richiesto
                        if(pub.postoDisponibile(ntavolo)){ //Verifica se ci sono posti disponibili al tavolo richiesto
                            pub.aggiungiCliente(ntavolo); //aggiunge il cliente al tavolo richiesto
                            clientSocket.send(message); //invia un messaggio di avviso al cameriere di posti disponibili
                        }
                        else{ //Se non ci sono posti
                            clientSocket.send("Tavolo inesistente o posti non disponibili"); //avvisa il camerire
                        }
                    //Eccezioni in caso di errore della conversione della stringa in intero
                    } catch (const invalid_argument& e) { clientSocket.send("Messaggio non valido: non è un numero");}
                }
            }
            //Avvisa il cameriere se nel Pub non ci sono posti
            else{
                if (!clientSocket.send("No")) {
                    break; // Errore nell'invio
                }
            }
        }
    }

    return 0;
}

// Signal handler per l'interruzione del programma
void signalHandler(int signum) {
    if (serverSocketPtr != nullptr) { // se ho il contenuto della socket
        serverSocketPtr->close(); //chiudo la socket
        cout << "Server socket closed successfully." << endl; // Stampa di successo
    }
    exit(signum); // esco dal programma in base al codice passato
}
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

        cout << "Connessione accettata!" << endl; // stampa un messaggio di successo della connessione con il client

        while (true){

            string message; // memorizzo i messaggi che arrivano dal client

            if (clientSocket.receive(message) <= 0) { // se il messaggio non viene ricevuto correttamente
                break; // Connessione chiusa o errore ed esco dal ciclo
            }

            cout << "Messaggio ricevuto: " << message << endl;

            /*if (!clientSocket.send("Messaggio ricevuto da te: " + message)) {
                    cerr << "Errore nell'invio della risposta!" << endl;
                    break; // Errore nell'invio
            }*/
        }

        clientSocket.close(); //chiusura del socket client
    }

    serverSocket.close(); //chiusura del socket server

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
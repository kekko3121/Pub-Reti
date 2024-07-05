#include <iostream>
#include <cstring>
#include "../Socket/Socket.h"
using namespace std;

int main() {
    Socket clientSocket; // dichiarazione della socket client

    // Definizione delle variabili
    int serverPort = 25562; // Porta su cui il server è in ascolto
    string remoteHost = "127.0.0.1"; // Indirizzo IP del server remoto

    // Creazione della socket server
    if (!clientSocket.create()) {
        cerr << "Errore nella creazione del socket del client!" << endl;
        return 1;
    }

    // Connessione al server remoto (Cameriere)
    if (!clientSocket.connect(remoteHost, serverPort)) {
        cerr << "Errore nella connessione al Cameriere!" << endl;
        return 1;
    }

    cout << "Connessione al server riuscita!" << endl; // Stampa di successo

    string message; // memorizzo i messaggi che arrivano dal server
    if (clientSocket.receive(message) <= 0) { // memorizzo i messaggi che arrivano dal client
        cerr << "Errore nella ricezione del messaggio!" << endl;
    }

    cout << "Messaggio ricevuto: " << message << endl; // Stampa il messaggio ricevuto

    // Inviare un messaggio al server
    if (!clientSocket.send("Ciao Cameriere!")) {
        cerr << "Errore nell'invio del messaggio al Cameriere!" << endl;
    }

    cout << "Messaggio inviato al server!" << endl;
    
    /*while (true) {

    }*/

    return 0;
}
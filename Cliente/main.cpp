#include <iostream>
#include <string>
#include "../Socket/Socket.h"
using namespace std;

bool numero_valido(int num, int min, int max);

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

    cout << "Sono entrato nel pub e mi sta servendo un cameriere" << endl; // Stampa di successo

    string message; // memorizzo i messaggi che arrivano dal server
    if (clientSocket.receive(message) <= 0) { // memorizzo i messaggi che arrivano dal client
        cerr << "Errore nella ricezione del messaggio!" << endl;
    }

    cout << "Cameriere: " << message << endl; // Stampa il messaggio ricevuto

    cout << "Cliente: Chiedo se è possibile accomodarsi..." << endl;
    sleep(2);

    // Inviare un messaggio al server
    // Inviare un messaggio al server
    if (!clientSocket.send("Posso entrare?")) {
        cerr << "Errore nell'invio del messaggio al Cameriere!" << endl;
    }

    message.clear();

    if (!clientSocket.receive(message)) {
        cerr << "Errore nella ricezione del messaggio!" << endl;
    }

    cout << "Cameriere: " << message << endl;

    if (message.compare("Mi dispiace ma non ci sono posti disponibili, Arrivederci") == 0) {
        clientSocket.close();
        exit(0);
    }

    string sendmessage; // variabile per memorizzare il messaggio del client

    cin >> sendmessage;

    if (!clientSocket.send(sendmessage)) {
        cerr << "Errore nell'invio del messaggio!" << endl;
    }

    message.clear();
    if (!clientSocket.receive(message)) {
        cerr << "Errore nella ricezione del messaggio!" << endl;
    }

    cout << "Cameriere: " << message << endl;

    message.clear();
    if (!clientSocket.receive(message)) {
        cerr << "Errore nella ricezione del messaggio!" << endl;
    }

    cout << "Cameriere: " << message << endl;

    // Se, invece, ci sono tavoli disponibili, procedo con l'ordinazione e la consegno al cameriere
    cout << "Inserisci il numero della portata desiderata (da 1 a 14) e conferma con INVIO.\n Per terminare l'ordine, digita 'exit'." << endl;

    // Inizializza una stringa per contenere l'ordine
    string order;
    // Stringa per l'input dell'utente
    string scelta;

    // Ciclo per acquisire l'ordine dall'utente
    do {
        cout << "Portata: ";
        cin >> scelta;

        // Controlla se il cliente vuole concludere l'ordine
        if (scelta.compare("exit") == 0) {
            // Se il cliente tenta di uscire senza aver ordinato nulla, chiedi di ordinare almeno una portata
            if (order.length() == 0) {
                cout << "Devi ordinare almeno una portata." << endl;
                continue; // Torna all'inizio del ciclo per richiedere la portata
            }
            break;
        }

        if (!numero_valido(stoi(scelta), 1, 14)) {
            cout << "Inserisci un numero compreso tra 1 e 14." << endl;
            continue;
        }

        order += scelta;
        order += " ";
    } while (1);

    // Invio l'ordine al cameriere
    if (!clientSocket.send(order)) {
        cerr << "Errore nell'invio dell'ordine al cameriere!" << endl;
    } else {
        cout << "Hai consegnato il tuo ordine al cameriere, ora verrà consegnato al pub per la preparazione." << endl;
    }

    return 0;
}

// Funzione per verificare se la portata inserita è corretta
bool numero_valido(int num, int min, int max) {
    return num >= min && num <= max;
}
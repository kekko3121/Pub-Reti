#include <iostream>
#include <cstdlib>
#include <string>
#include <signal.h>
#include "../Socket/Socket.h"
using namespace std;

//funzione per verificare se il range del numero è valido
bool numero_valido(int num, int min, int max);

// Signal handler per l'interruzione del programma
void signalHandler(int);

// Funzione per gestire i messaggi in caso di SIGNIT
void termine_client(Socket *, string *);

// Socket globale per il signal handling
Socket* clientSocketPtr = nullptr;

int main() {
    Socket clientSocket; // dichiarazione della socket client
    string sendmessage; // variabile per memorizzare il messaggio del client per la scelta del tavolo
    int serverPort = 25562; // Porta su cui il server è in ascolto
    string remoteHost = "127.0.0.1"; // Indirizzo IP del server remoto

    // Creazione della socket server
    if (!clientSocket.create()) {
        cerr << "Errore nella creazione del socket del client!" << endl;
        exit(1); // Terminazione del programma
    }

    // Connessione al server remoto (Cameriere)
    if (!clientSocket.connect(remoteHost, serverPort)) {
        cerr << "Errore nella connessione al Cameriere!" << endl;
        exit(1); // Terminazione del programma
    }

    // Registrazione del signal handler
    signal(SIGINT, signalHandler);
    clientSocketPtr = &clientSocket; // passo l'indirizzo di memoria della variabile clientSocket

    cout << "Sono entrato nel pub e mi sta servendo un cameriere" << endl; // Stampa di successo

    string message; // memorizza i messaggi che arrivano dal server
    if (clientSocket.receive(message) <= 0) { // rivere il benvenuto dal cameriere
        cerr << "Errore nella ricezione del messaggio!" << endl;
    }

    cout << "Cameriere: " << message << endl; // Stampa il messaggio ricevuto

    cout << "Cliente: Chiedo se è possibile accomodarsi..." << endl; // Stampa di avviso
    sleep(2); // attesa di 2 secondi

    // Invia un messaggio al Cameriere per chiedere se possibile accomodarsi
    if (!clientSocket.send("Posso entrare?")) {
        cerr << "Errore nell'invio del messaggio al Cameriere!" << endl;
        exit(1); // Terminazione del programma 
    }

    message.clear(); // cancella il contenuto della variabile messagge

    // Riceve la risposta dal cameriere
    if (clientSocket.receive(message) <= 0) {
        cerr << "Errore nella ricezione del messaggio!" << endl;
        exit(1); // Terminazione del programma
    }

    termine_client(&clientSocket, &message); // Se il cliente o il pub si disconnettono senza preavviso

    cout << "Cameriere: " << message << endl; // Stampa la risposta

    // Se non ci sono posti
    if (message.compare("Mi dispiace ma non ci sono posti disponibili, Arrivederci") == 0) {
        clientSocket.close(); // mi disconnetto
        exit(1); // Terminazione del programma
    }

    bool valid = false; // variabile per verificare il corretto inserimento della scelta

    do {
        cin >> sendmessage; // Inserisce la scelta

        // verifica che la stringa inserita sia uguale a "nuovo"
        if (sendmessage == "nuovo") {
            valid = true; // imposta la variabile a true
        }

        // verifica se in input e stato inserito un numero intero
        else {
            try {
                // converte il numero in intero
                int number = stoi(sendmessage);
                valid = true;  // imposta la variabile a true
            } catch (invalid_argument&) {
                // Se la conversione fallisce
                cout << "Inserisci 'nuovo' o un numero di tavolo valido" << endl; // Stampa una notifica di errore
            }
        }
    } while (!valid); // ripete finche la variabile è false

    if (!clientSocket.send(sendmessage)) { // invia il messaggio al camerire della scelta del tavolo
        cerr << "Errore nell'invio del messaggio!" << endl;
        exit(1); // Terminazione del programma
    }

    message.clear(); // cancella il contenuto della variabile messagge

    if (clientSocket.receive(message) <= 0) { // riceve la risposta dal camerire per il tavolo scelto
        cerr << "Errore nella ricezione del messaggio!" << endl;
        exit(1); // Terminazione del programma
    }

    termine_client(&clientSocket, &message); // Se il cliente o il pub si disconnettono senza preavviso

    cout << "Cameriere: " << message << endl; // Stampo il messaggio ricevuto

    if(message.compare("Tavolo non disponibile") == 0){ // Se il tavolo scelto non è disponibile abbandono il Pub
        clientSocket.close(); //mi disconnetto
        exit(1); // Terminazione del programma
    }

    cout << "Cliente: Richiedo il menu al cameriere" << endl; // Stampa di avviso di richiesta menu

    if (!clientSocket.send("Mi porti il menu")) { // Invia la richiesta del menu al cameriere
        cerr << "Errore nell'invio del messaggio!" << endl;
        exit(1); // Terminazione del programma
    }

    sleep(5); // attesa di simulazione

    message.clear(); // cancella il contenuto della variabile messagge

    if (clientSocket.receive(message) <= 0 ) { // riceve il menu
        cerr << "Errore nella ricezione del messaggio!" << endl;
        exit(1); // Terminazione del programma
    }

    termine_client(&clientSocket, &message); // Se il cliente o il pub si disconnettono senza preavviso

    cout << "Cameriere: " << message << endl; // Stampa il menu ricevuto

    //Effettua l'ordinazione in base al menu consegnato
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
            // Se il cliente tenta di uscire senza aver ordinato nulla, chiede di ordinare almeno una portata
            if (order.length() == 0) {
                cout << "Devi ordinare almeno una portata." << endl;
                continue; // Torna all'inizio del ciclo per richiedere la portata
            }
            break;
        }

        if (!numero_valido(stoi(scelta), 1, 14)) { // verifica dei numeri nel range del menu
            cout << "Inserisci un numero compreso tra 1 e 14." << endl;
            continue; // Torna all'inizio della richiesta della portata
        }

        order += scelta; //aggiunge la scelta all'ordine
        order += " "; //aggiunge uno spazio per separare le ordinazioni
    } while (1);

    // Invia l'ordine al cameriere
    if (!clientSocket.send(order)) {
        cerr << "Errore nell'invio dell'ordine al cameriere!" << endl;
        exit(1); // Terminazione del programma
    } else {
        cout << "Hai consegnato il tuo ordine al cameriere, ora verrà consegnato al pub per la preparazione." << endl;
    }

    message.clear(); // cancella il contenuto della variabile messagge

    if (clientSocket.receive(message) <= 0 ) { // riceve l'ordine pronto
        cerr << "Errore nella ricezione del messaggio!" << endl;
        exit(1); // Terminazione del programma
    }

    termine_client(&clientSocket, &message); // Se il cliente o il pub si disconnettono senza preavviso

    cout << "Cameriere: " << message << endl;

    if(!clientSocket.send("Grazie mille e arrivederci")){ // invia ringraziamenti al cameriere
        cerr << "Errore nell'invio del ringraziamento al cameriere!" << endl;
        exit(1); // Terminazione del programma
    }

    return 0;
}

// Funzione per verificare se la portata inserita è corretta
bool numero_valido(int num, int min, int max) {
    return num >= min && num <= max;
}

/* Funzione di controllo terminazione programma */
void signalHandler(int signum) {
    //chiusura della socket client
    if (clientSocketPtr != nullptr) {

        if(!clientSocketPtr->send("termine_cliente")){ // avvisa il cameriere della terminazione
            cerr << "Errore nell'invio del messaggio al Cameriere!" << endl;
        }
        
        clientSocketPtr->close(); // chiude la socket
        std::cout << "Client socket closed successfully." << std::endl; // stampa di avviso
    }

    exit(signum); //uscita dal programma
}

// Funzione per gestire i messaggi in caso di SIGNIT
void termine_client(Socket *sock, string *message) {
    if (message->compare("termine_pub") == 0) {
        cout << "Il pub si è disconnesso improvvisamente! Si prega di riavviarlo." << endl; // Stampa di errore
        sock->close();
        exit(0);
    } else if (message->compare("termine_cameriere") == 0) {
        cout << "Il cameriere si è disconnesso improvvisamente!" << endl; // Stampa di errore
        sock->close();
        exit(0);
    }
}

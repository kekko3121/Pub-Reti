#include <iostream>
#include <string>
#include <signal.h>
#include "Pub.h"
#include "../Socket/Socket.h"
using namespace std;

// Global socket pointer for signal handling
Socket* serverSocketPtr = nullptr;

// Signal handler for clean shutdown
void signalHandler(int);

int main() {
    Socket serverSocket;

    // Initialize Pub with 5 tables and max 20 seats
    Pub pub(20, 5);

    pub.aggiungiTavolo(4);
    pub.aggiungiTavolo(6);
    pub.aggiungiTavolo(2);
    pub.aggiungiTavolo(5);
    pub.aggiungiTavolo(2);

    // Set up signal handler
    signal(SIGINT, signalHandler);
    serverSocketPtr = &serverSocket;

    // Create the server socket
    if (!serverSocket.create()) {
        cerr << "Errore nella creazione del socket!" << endl;
        return -1;
    }

    // Bind the socket to a port (e.g., 25564)
    if (!serverSocket.bind(25564)) {
        cerr << "Errore nell'associazione del socket alla porta!" << endl;
        return -1;
    }

    // Put the socket in listening mode
    if (!serverSocket.listen()) {
        cerr << "Errore nel mettere il socket in ascolto!" << endl;
        return -1;
    }

    cout << "Il pub è aperto in attesa di clienti" << endl;

    while (true) {

        // Accept an incoming connection
        Socket clientSocket;
        if (!serverSocket.accept(clientSocket)) {
            cerr << "Errore nell'accettare la connessione!" << endl;
            return -1;
        }

        cout << "Connessione accettata!" << endl;

        while (true){
            // Receive a message from the client
            string message;

            if (clientSocket.receive(message) <= 0) {
                break; // Connessione chiusa o errore
            }

            cout << "Messaggio ricevuto: " << message << endl;

            /*if (!clientSocket.send("Messaggio ricevuto da te: " + message)) {
                    cerr << "Errore nell'invio della risposta!" << endl;
                    break; // Errore nell'invio
            }*/
        }
    }
    
    return 0;
}

// Signal handler for clean shutdown
void signalHandler(int signum) {
    if (serverSocketPtr != nullptr) {
        serverSocketPtr->close();
        cout << "Server socket closed successfully." << endl;
    }
    exit(signum);
}
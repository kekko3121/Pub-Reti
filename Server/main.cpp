#include <iostream>
#include <string>
#include <signal.h>
#include "Pub.h"
#include "../Socket/Socket.h"

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
        std::cerr << "Errore nella creazione del socket!" << std::endl;
        return -1;
    }

    // Bind the socket to a port (e.g., 25564)
    if (!serverSocket.bind(25564)) {
        std::cerr << "Errore nell'associazione del socket alla porta!" << std::endl;
        return -1;
    }

    // Put the socket in listening mode
    if (!serverSocket.listen()) {
        std::cerr << "Errore nel mettere il socket in ascolto!" << std::endl;
        return -1;
    }

    std::cout << "Il pub è aperto in attesa di clienti" << std::endl;

    while (true) { // Stay in listening mode to accept multiple connections
        // Accept an incoming connection
        Socket newSocket;
        if (!serverSocket.accept(newSocket)) {
            std::cerr << "Errore nell'accettare la connessione!" << std::endl;
            continue;
        }

        std::cout << "Connessione accettata!" << std::endl;

        // Receive a message from the client
        std::string message;
        if (newSocket.receive(message) > 0) {
            std::cout << "Messaggio ricevuto: " << message << std::endl;

            // Handle the message, e.g., process client requests, manage orders, etc.
            // For now, just send a response
            std::string response = "Messaggio ricevuto: " + message;
            if (!newSocket.send(response)) {
                std::cerr << "Errore nell'invio della risposta!" << std::endl;
            }
        } else {
            std::cerr << "Errore nella ricezione del messaggio!" << std::endl;
        }
    }

    return 0;
}

// Signal handler for clean shutdown
void signalHandler(int signum) {
    if (serverSocketPtr != nullptr) {
        serverSocketPtr->close();
        std::cout << "Server socket closed successfully." << std::endl;
    }
    exit(signum);
}
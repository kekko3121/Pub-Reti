#include <iostream>
#include <string>
#include "Pub.h"
#include "../Socket/Socket.h"

int main() {
    Socket serverSocket;

   Pub pub = Pub(20, 5);

   pub.aggiungiTavolo(4);
   pub.aggiungiTavolo(6);
   pub.aggiungiTavolo(2);
   pub.aggiungiTavolo(5);
   pub.aggiungiTavolo(2);
    
    // Creare il socket
    if (!serverSocket.create()) {
        std::cerr << "Errore nella creazione del socket!" << std::endl;
        return -1;
    }

    // Associare il socket ad una porta (es. 8080)
    if (!serverSocket.bind(25563)) {
        std::cerr << "Errore nell'associazione del socket alla porta!" << std::endl;
        return -1;
    }

    // Mettere il socket in ascolto
    if (!serverSocket.listen()) {
        std::cerr << "Errore nel mettere il socket in ascolto!" << std::endl;
        return -1;
    }

    std::cout << "Il pub è aperto in attesa di clienti" << std::endl;

    while(true){ // Rimane in ascolto per accettare più connessioni
        // Accettare una connessione in entrata
        Socket newSocket;
        if (!serverSocket.accept(newSocket)) {
            std::cerr << "Errore nell'accettare la connessione!" << std::endl;
            return -1;
        }

        std::cout << "Connessione accettata!" << std::endl;

        // Ricevere un messaggio dal client
        std::string message;
        if (newSocket.receive(message) > 0) {
            std::cout << "Messaggio ricevuto: " << message << std::endl;

            /*if (!newSocket.send("response")) {
                std::cerr << "Errore nell'invio della risposta!" << std::endl;
            }*/
        } else {
            std::cerr << "Errore nella ricezione del messaggio!" << std::endl;
        }
    }

    return 0;
}
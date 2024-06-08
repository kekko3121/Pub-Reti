#include "../Socket/Socket.h"

int main() {
    Socket clientSocket;

    // Creare il socket
    if (!clientSocket.create()) {
        std::cerr << "Errore nella creazione del socket del client!" << std::endl;
        return 1;
    }

    // Connettersi al server
    if (!clientSocket.connect("127.0.0.1", 25562)) {
        std::cerr << "Errore nella connessione al server!" << std::endl;
        return 1;
    }

    std::cout << "Connessione al server riuscita!" << std::endl;

    while (true) {
        // Inviare un messaggio al server
        std::string message = "Ciao server!";
        if (!clientSocket.send(message)) {
            std::cerr << "Errore nell'invio del messaggio al server!" << std::endl;
            break; // Esci dal loop se c'è un errore nell'invio
        }

        std::cout << "Messaggio inviato al server!" << std::endl;

        // Ricevere la risposta dal server
        std::string response;
        int status = clientSocket.receive(response);
        if (status > 0) {
            std::cout << "Risposta dal server: " << response << std::endl;
        } else {
            std::cerr << "Errore nella ricezione della risposta dal server!" << std::endl;
            break; // Esci dal loop se c'è un errore nella ricezione
        }
    }

    return 0;
}
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

    // Ricevere la risposta dal server
    std::string message;
    if (clientSocket.receive(message) <= 0) {
        std::cerr << "Errore nella ricezione del messaggio!" << std::endl;
    }

    std::cout << "Messaggio ricevuto: " << message << std::endl;

    // Inviare un messaggio al server
    if (!clientSocket.send("Ciao server!")) {
        std::cerr << "Errore nell'invio del messaggio al server!" << std::endl;
    }

    std::cout << "Messaggio inviato al server!" << std::endl;
    
    /*while (true) {

    }*/

    return 0;
}
#include "../Socket/Socket.h"
#include <unistd.h> // per fork()

void serverProcess(Socket clientSocket) {
    while (true) {
        // Ricevere la richiesta dal client
        std::string request;
        int status = clientSocket.receive(request);
        if (status > 0) {
            std::cout << "Messaggio dal client: " << request << std::endl;

            // Esempio di risposta al client
            std::string response = "Ricevuto messaggio: " + request;
            clientSocket.send(response);
        } else {
            std::cerr << "Errore nella ricezione della richiesta dal client!" << std::endl;
            break; // Esci dal loop se c'è un errore nella ricezione
        }
    }
}

int main() {
    Socket clientSocket, serverSocket;

    // Creare il socket
    if (!clientSocket.create()) {
        std::cerr << "Errore nella creazione del socket del client!" << std::endl;
        return 1;
    }

    // Connettersi al server
    if (!clientSocket.connect("127.0.0.1", 25563)) {
        std::cerr << "Errore nella connessione al server!" << std::endl;
        return 1;
    }

    std::cout << "Connessione al server riuscita!" << std::endl;

    if (!serverSocket.create()) {
        std::cerr << "Errore nella creazione del socket del client!" << std::endl;
        return 1;
    }

     if (!serverSocket.bind(25562)) {
        std::cerr << "Errore nel bind del socket per il server locale!" << std::endl;
        return 1;
    }

    if (!serverSocket.listen()) {
        std::cerr << "Errore nell'ascolto delle connessioni per il server locale!" << std::endl;
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        // Errore nella creazione del processo figlio
        std::cerr << "Errore nella creazione del processo figlio!" << std::endl;
        return 1;

    }
    else if (pid == 0) {
       // Processo figlio
        Socket clientConnection;
        if (serverSocket.accept(clientConnection)) {
            std::cout << "Connessione accettata dal client locale!" << std::endl;
            serverProcess(clientConnection);
        }
        else {
            std::cerr << "Errore nell'accettare la connessione dal client locale!" << std::endl;
        } 
    }
    else {
        // Processo padre
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
    }

    return 0;
}

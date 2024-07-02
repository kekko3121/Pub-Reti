#include "../Socket/Socket.h"
#include <unistd.h>
#include <signal.h>

// Global socket pointers for signal handling
Socket* clientSocketPtr = nullptr;
Socket* serverSocketPtr = nullptr;

// Signal handler for clean shutdown
void signalHandler(int);
void serverProcess(Socket);

int main() {
    // Definizione delle variabili
    int serverPort = 25562; // Porta su cui il server in ascolto
    string remoteHost = "127.0.0.1"; // Indirizzo IP del server remoto
    int remotePort = 25564; // Porta del server remoto

    // Creazione del socket server
    Socket serverSocket;
    if (!serverSocket.create()) {
        cerr << "Errore nella creazione del socket server" << endl;
        return 1;
    }

    // Bind del socket server alla porta specificata
    if (!serverSocket.bind(serverPort)) {
        cerr << "Errore nel bind del socket server" << endl;
        return 1;
    }

    // Mettere in ascolto il socket server
    if (!serverSocket.listen()) {
        cerr << "Errore nel mettere in ascolto il socket server" << endl;
        return 1;
    }

    cout << "Server in ascolto sulla porta " << serverPort << endl;

    // Creazione del socket per la connessione remota
    Socket remoteSocket;

    if (!remoteSocket.create()) {
        cerr << "Errore nella creazione del socket server" << endl;
        return 1;
    }

    // Connessione al server remoto
    if (!remoteSocket.connect(remoteHost, remotePort)) {
        cerr << "Errore nella connessione al server remoto: " << strerror(errno) << endl;
        return 1;
    }

    cout << "Connesso al server remoto: " << remoteHost << ":" << remotePort << endl;

    // Registrazione del signal handler
    signal(SIGINT, signalHandler);

    while (true) {
        // Accetta una nuova connessione dal client
        Socket clientSocket;
        if (!serverSocket.accept(clientSocket)) {
            cerr << "Errore nell'accettazione della connessione client" << endl;
            continue;
        }

        cout << "Connessione client accettata" << endl;

        // Crea un processo figlio per gestire la connessione client
        pid_t pid = fork();
        if (pid == 0) { // Processo figlio
            // Chiude il descrittore del socket server nel processo figlio
            if (!serverSocket.close()){
                cerr << "Errore nella chiusura del socket server" << endl;
            }

            // Gestisce la comunicazione con il client
            while (true) {
                string message;

                if (clientSocket.receive(message) <= 0) {
                    break; // Connessione chiusa o errore
                }

                cout << "Messaggio ricevuto dal client: " << message << endl;

                // Invia una risposta al client
                if (!clientSocket.send("Messaggio ricevuto dal server")) {
                    break; // Errore nell'invio
                }
            }

            // Chiude il socket client nel processo figlio
            clientSocket.close();

            cout << "Connessione client terminata" << endl;

            exit(0); // Termina il processo figlio
        } else if (pid > 0) { // Processo padre
            // Continua ad accettare nuove connessioni client
            continue;
        } else { // Errore nella fork
            cerr << "Errore nella creazione del processo figlio" << endl;
            break;
        }
    }

    // Chiude il socket server nel processo padre
    serverSocket.close();

    return 0;
}

void signalHandler(int signum) {
    if (clientSocketPtr != nullptr) {
        clientSocketPtr->close();
        std::cout << "Client socket closed successfully." << std::endl;
    }
    if (serverSocketPtr != nullptr) {
        serverSocketPtr->close();
        std::cout << "Server socket closed successfully." << std::endl;
    }
    exit(signum);
}
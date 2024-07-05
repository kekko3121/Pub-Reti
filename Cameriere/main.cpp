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
    Socket serverSocket, remoteSocket; // Creazione dei socket per il collegamento
    
    if (!serverSocket.create()) {
        cerr << "Errore nella creazione del socket server" << endl;
        return -1;
    }

    // Bind del socket server alla porta specificata
    if (!serverSocket.bind(serverPort)) {
        cerr << "Errore nel bind del socket server" << endl;
        return -1;
    }

    // Mettere in ascolto il socket server
    if (!serverSocket.listen()) {
        cerr << "Errore nel mettere in ascolto il socket server" << endl;
        return -1;
    }

    if (!remoteSocket.create()) {
        cerr << "Errore nella creazione del socket server" << endl;
        return -1;
    }

    // Connessione al server remoto
    if (!remoteSocket.connect(remoteHost, remotePort)) {
        cerr << "Errore nella connessione al server remoto: " << strerror(errno) << endl;
        return -1;
    }

    cout << "Ho iniziato il turno nel Pub delle socket" << endl;

    // Registrazione del signal handler
    signal(SIGINT, signalHandler);

    // Set global pointers for signal handling
    serverSocketPtr = &serverSocket;
    clientSocketPtr = &remoteSocket;

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

            // Gestisce la comunicazione con il client
            while (true) {

                // Invia una risposta al client
                if (!clientSocket.send("Ciao, Benvenuto nel pub socket")) {
                    break; // Errore nell'invio
                }

                string message;

                if (clientSocket.receive(message) <= 0) {
                    break; // Connessione chiusa o errore
                }

                cout << "Messaggio ricevuto dal client: " << message << endl;

                // Invia una risposta al server
                if (!remoteSocket.send("il cliente ha inviato: " + message)) {
                    break; // Errore nell'invio
                }
            }

            // Chiude il socket client nel processo figlio
            clientSocket.close();

            cout << "Il cliente ha abbandonato il pub" << endl;

            exit(0); // Termina il processo figlio
        }
        
        else if (pid > 0) { // Processo padre
            // Continua ad accettare nuove connessioni client
            continue;
        }
        
        else { // Errore nella fork
            cerr << "Errore nella creazione del processo figlio" << endl;
            
            // Chiude il descrittore del socket server nel processo figlio
            if (!serverSocket.close()){
                cerr << "Errore nella chiusura del socket server" << endl;
            }

            break;
        }
    }

    // Chiude il socket server nel processo padre
    serverSocket.close();
    remoteSocket.close();

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
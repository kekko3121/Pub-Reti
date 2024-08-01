#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <string>
#include "../Socket/Socket.h"
using namespace std;

// Socket globale per il signal handling
Socket* clientSocketPtr = nullptr;
Socket* serverSocketPtr = nullptr;

// Signal handler per l'interruzione del programma
void signalHandler(int);
// Funzione per inviare il menu al cameriere 
void send_menu(Socket*);

int main() {
    // Definizione delle variabili
    int serverPort = 25562; // Porta su cui il server in ascolto
    string remoteHost = "127.0.0.1"; // Indirizzo IP del server remoto
    int remotePort = 25564; // Porta del server remoto
    Socket serverSocket; // dichiarazione della socket server e client

    // Creazione della socket server
    if (!serverSocket.create()) {
        cerr << "Errore nella creazione del socket server" << endl;
        return -1;
    }

    // Costruisce la socket con la porta 25564
    if (!serverSocket.bind(serverPort)) {
        cerr << "Errore nel bind del socket server" << endl;
        return -1;
    }

    // Imposta la socket in ascolto
    if (!serverSocket.listen()) {
        cerr << "Errore nel mettere in ascolto il socket server" << endl;
        return -1;
    }

    cout << "Ho iniziato il turno nel Pub delle socket" << endl; // Stampa di funzionamento della socket

    // Registrazione del signal handler
    signal(SIGINT, signalHandler);
    serverSocketPtr = &serverSocket;

    while (true) {

        Socket clientSocket, remoteSocket; //socket per acquisire le informazioni del client
        clientSocketPtr = &remoteSocket;
        int ntavolo;

        // Accetta una nuova connessione dal client
        if (!serverSocket.accept(clientSocket)) {
            cerr << "Errore nell'accettazione della connessione client" << endl;
            continue;
        }

        if (!remoteSocket.create()) {
            cerr << "Errore nella creazione del socket server" << endl;
            return -1;
        }

        cout << "E entrato un cliente nel Pub" << endl;

        // Crea un processo figlio per gestire la connessione client
        pid_t pid = fork();

        if(pid < 0){
            cerr << "Errore nella fork";
            continue;
        }

        if (pid == 0) { // Processo figlio
            string message; // memorizzo i messaggi che arrivano dal client
            // Creazione della socket client per il server remoto

            // Connessione al server remoto (Pub)
            if (!remoteSocket.connect(remoteHost, remotePort)) {
                cerr << "Errore nella connessione al server remoto: " << strerror(errno) << endl;
                return -1;
            }

            // Invia una risposta al client
            if (!clientSocket.send("Ciao, Benvenuto nel pub socket")) {
                break; // Errore nell'invio
            }


            if (clientSocket.receive(message) <= 0) {
                break; // Connessione chiusa o errore
            }

            cout << "Cliente: " << message << endl;

            // Se il cliente vuole accomodarsi
            if(message.compare("Posso entrare?") == 0){
                remoteSocket.send("Ci sono posti liberi?"); //chiedo al pub se ci sono posti disponibili

                message.clear();

                remoteSocket.receive(message);

                if(message.compare("Si") == 0){ //Se ci sono posti
                    //chiedo al cliente dove vuole accomodarsi
                    clientSocket.send("Vuole accomodarsi a un nuovo tavolo o ad uno già prenotato? \n Scriva nuovo o numero tavolo");

                    message.clear();

                    clientSocket.receive(message);

                    if(message.compare("nuovo") != 0){ //se decide di accomodarsi ad uno già occupato
                        remoteSocket.send(message); //chiedo al pub un nuovo tavolo disponibili
                    }

                    else{ //se il cliente decide un nuovo tavolo 
                        remoteSocket.send(message); //chiedo al pub posti disponibili al tavolo scelto
                    }

                    message.clear();
                    remoteSocket.receive(message);

                    try{ //eccezione se viene richiesto un tavolo non esistente
                        ntavolo = stoi(message); //memorizzo il numero di tavolo
                        clientSocket.send("Accomodati al tavolo numero: " + message); //invio il numero di tavolo al cliente
                    }
                    catch (const invalid_argument& e) {//se non esiste il numero di tavolo
                        clientSocket.send("Tavolo non disponibile"); //avvisa il cliente
                        break;
                    }

                    message.clear();
                    clientSocket.receive(message); //riceve la richiesta di consegna menu dal cliente

                    if(message.compare("Mi porti il menu") == 0){ //Se il cliente richiede il menu
                        cout << "Porto il menu al tavolo " << ntavolo << endl; //stampa di avviso
                        send_menu(&clientSocket); //richiama la funzione per inviare il menu al cliente
                        message.clear();
                        clientSocket.receive(message); //riceve l'ordine effettuato dal cliente
                        cout << "ho acquisito l'ordine e lo trasmetto al pub" << endl; //stampa di avviso
                        remoteSocket.send("Prepara ordine " + to_string(ntavolo)); //invia l'ordinazione al Pub
                        message.clear();
                        remoteSocket.receive(message); //riceve il messaggio di ordine preparato
                        cout << "Pub: " << message << endl; //Stampa il messaggio
                        if(message.substr(0, 13).compare("Ordine pronto") == 0){ //Se l'ordine è pronto
                            cout << " Ritiro il menu e lo consegno al tavolo" + ntavolo << endl; //viene ritirato
                            clientSocket.send("Ordine pronto e consegnato"); //l'ordine viene consegnato al cliente
                        }

                        message.clear();

                        clientSocket.receive(message); //il camerire riceve ringraziamenti dal cliente

                        cout << "Cliente: " << message << endl; //Stampa il messaggio del cliente

                        message.clear();
                    }
                }
            }

            cout << "Il cliente ha abbandonato il pub" << endl; // Stampa di successo

            remoteSocket.send("Cliente ha liberato il tavolo n: " + to_string(ntavolo));

            exit(0); // Termina il processo figlio
        }
    }

    return 0;
}

/* Funzione di controllo terminazione programma */
void signalHandler(int signum) {
    //chiusura della socket client
    if (clientSocketPtr != nullptr) {
        clientSocketPtr->close();
        std::cout << "Client socket closed successfully." << std::endl;
    }
    //chiusura della socket server
    if (serverSocketPtr != nullptr) {
        serverSocketPtr->close();
        std::cout << "Server socket closed successfully." << std::endl;
    }
    exit(signum); //uscita dal programma
}

/* Funzione per inviare il menu al cameriere */
void send_menu(Socket* client) {
    // menu del Pub
    string message = R"(Menu Pub
                        1) Fourier small menu
                        2) ADSL large menu
                        3) Fibra medium menu
                        4) ALOHA single hamburger
                        5) Ethernet vegan menu small
                        6) Berkeley Socket menu
                        7) UDP BBQ menu
                        8) DNS Nuggets x6
                        9) DNS Nuggets x12
                        10) LAN Wrap menu
                        11) MultiplexingCola
                        12) Gran P2P Bacon menu
                        13) ISO Water
                        14) RFID Fanta
                        )";

    if (!client->send(message)) { //invia il menu al cliente
        cerr << "Errore nell'invio del menu al cliente" << endl;
    }
}
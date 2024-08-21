#include <iostream>
#include <cstdlib>
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

// Funzione del cameriere in caso di SIGNIT
void termine_cameriere(Socket *, Socket *, string *);

int main() {

    int serverPort = 25562; // Porta di ascolto del server
    string remoteHost = "127.0.0.1"; // Indirizzo IP del server remoto
    int remotePort = 25564; // Porta del server remoto
    Socket serverSocket; // dichiarazione della socket server

    // Creazione della socket server
    if (!serverSocket.create()) {
        cerr << "Errore nella creazione del socket server" << endl;
        exit(1); // In caso di errore esco dal programma
    }

    // Costruisce la socket con la porta remotePort
    if (!serverSocket.bind(serverPort)) {
        cerr << "Errore nel bind del socket server" << endl;
        exit(1);
    }

    // Imposta la socket in ascolto
    if (!serverSocket.listen()) {
        cerr << "Errore nel mettere in ascolto il socket server" << endl;
        exit(1);
    }

    cout << "Ho iniziato il turno nel Pub delle socket" << endl; // Stampa di funzionamento della socket

    // Registrazione del signal handler
    signal(SIGINT, signalHandler);
    serverSocketPtr = &serverSocket; // passo l'indirizzo di memoria della variabile serverSocket

    while (true) {

        Socket clientSocket, remoteSocket; //socket per acquisire le informazioni del client e connessione al server (Pub)
        clientSocketPtr = &remoteSocket; // passo l'indirizzo di memoria della variabile remoteSocket
        int ntavolo;

        // Accetta una nuova connessione dal client
        if (!serverSocket.accept(clientSocket)) {
            cerr << "Errore nell'accettazione della connessione client" << endl;
            continue; // continua con il ciclo
        }

        if (!remoteSocket.create()) {
            cerr << "Errore nella creazione del socket server" << endl;
            continue; // continua con il ciclo
        }

        cout << "E entrato un cliente nel Pub" << endl;

        // Crea un processo figlio per gestire la connessione client
        pid_t pid = fork();

        if(pid < 0){ // Errore creazione processo figlio
            cerr << "Errore nella fork";
            continue; // continua con il ciclo
        }

        if (pid == 0) { // Processo figlio
            string message; // memorizzo i messaggi che arrivano dal client (cliente)

            // Connessione al server remoto (Pub)
            if (!remoteSocket.connect(remoteHost, remotePort)) {
                cerr << "Errore nella connessione al server remoto: " << strerror(errno) << endl;
                exit(1); // In caso di errore termino il processo figlio
            }

            // Invia un messaggio di benvenuto al client
            if (!clientSocket.send("Ciao, Benvenuto nel pub socket")) {
                cerr << "Errore nell'invio del messaggio al cliente!" << endl;
                exit(1); // Termina il processo figlio
            }


            if (clientSocket.receive(message) <= 0) { // richiesta di entrare dal cliente
                cerr << "Errore nella ricezione del messaggio dal cliente!" << endl;
                exit(1); // Termina il processo figlio
            }

            cout << "Cliente: " << message << endl;

            // Se il cliente vuole accomodarsi
            if(message.compare("Posso entrare?") == 0){
                if(!remoteSocket.send("Ci sono posti liberi?")){ // chiede al pub se ci sono posti disponibili
                    cerr << "Errore nell'invio del messaggio al Pub!" << endl;
                    exit(1); // Termina il processo figlio
                }

                message.clear(); // cancella il contenuto della variabile messagge

                if (remoteSocket.receive(message) <= 0) { // riceve la risposta della richiesta dei posti
                    cerr << "Errore nella ricezione del messaggio dal Pub!" << endl;
                    exit(1); // Termina il processo figlio
                }

                termine_cameriere(&remoteSocket, &clientSocket, &message); // Se il cliente o il pub si disconnettono senza preavviso

                if(message.compare("Si") == 0){ // Se ci sono posti
                    // chiede al cliente dove vuole accomodarsi
                    if(!clientSocket.send("Vuole accomodarsi a un nuovo tavolo o ad uno già prenotato? \n Scriva nuovo o numero tavolo")){
                        cerr << "Errore nell'invio del messaggio al cliente!" << endl;
                        exit(1); // Termina il processo figlio
                    }

                    message.clear(); // cancella il contenuto della variabile messagge

                    if (clientSocket.receive(message) <= 0) { // Riceve la richiesta del tavolo scelto
                        cerr << "Errore nella ricezione del messaggio dal Pub!" << endl;
                        exit(1); // Termina il processo figlio
                    }

                    termine_cameriere(&remoteSocket, &clientSocket, &message); // Se il cliente o il pub si disconnettono senza preavviso

                    if(!remoteSocket.send(message)){ // Invia la richiesta del tavolo scelto al Pub
                        cerr << "Errore nell'invio del messaggio al Pub!" << endl;
                        exit(1); // Termina il processo figlio
                    }

                    message.clear(); // cancella il contenuto della variabile messagge

                    if (remoteSocket.receive(message) <= 0) { // Riceva il numero di tavolo dal Pub
                        cerr << "Errore nella ricezione del messaggio dal Pub!" << endl;
                        exit(1); // Termina il processo figlio
                    }

                    termine_cameriere(&remoteSocket, &clientSocket, &message); // Se il cliente o il pub si disconnettono senza preavviso

                    try{ // eccezione se viene richiesto un tavolo non esistente
                        ntavolo = stoi(message); // memorizza il numero di tavolo
                        if(!clientSocket.send("Accomodati al tavolo numero: " + message)){ // invia il numero di tavolo al cliente
                            cerr << "Errore nell'invio del messaggio al cliente!" << endl;
                            exit(1);
                        }
                    }
                    catch (const invalid_argument& e) {// se non esiste il numero di tavolo
                        if(!clientSocket.send("Tavolo non disponibile")){ // avvisa il cliente
                            cerr << "Errore nell'invio del messaggio al cliente!" << endl;
                            exit(1); // Termina il processo figlio
                        }
                        exit(1); // Termina il processo figlio
                    }

                    message.clear(); // cancella il contenuto della variabile messagge

                    if (clientSocket.receive(message) <= 0) { // riceve la richiesta di consegna menu dal cliente
                        cerr << "Errore nella ricezione del messaggio dal client!" << endl;
                        exit(1); // Termina il processo figlio
                    }

                    termine_cameriere(&remoteSocket, &clientSocket, &message); // Se il cliente o il pub si disconnettono senza preavviso

                    if(message.compare("Mi porti il menu") == 0){ // Se il cliente richiede il menu

                        cout << "Porto il menu al tavolo " << ntavolo << endl; // stampa di avviso

                        send_menu(&clientSocket); // richiama la funzione per inviare il menu al cliente

                        message.clear(); // cancella il contenuto della variabile messagge

                        if (clientSocket.receive(message) <= 0) { // riceve l'ordine effettuato dal cliente
                            cerr << "Errore nella ricezione del messaggio dal client!" << endl;
                            exit(1); // Termina il processo figlio
                        }

                        termine_cameriere(&remoteSocket, &clientSocket, &message); // Se il cliente o il pub si disconnettono senza preavviso

                        cout << "ho acquisito l'ordine e lo trasmetto al pub" << endl; // stampa di avviso

                        if(!remoteSocket.send("Prepara ordine " + to_string(ntavolo))){ // invia l'ordinazione al Pub
                            cerr << "Errore nell'invio del messaggio al Pub!" << endl;
                            exit(1); // Termina il processo figlio
                        }

                        message.clear(); // cancella il contenuto della variabile messagge

                        if (remoteSocket.receive(message) <= 0) { // riceve il messaggio di ordine preparato
                            cerr << "Errore nella ricezione del messaggio dal client!" << endl;
                            exit(1); // Termina il processo figlio
                        }

                        termine_cameriere(&remoteSocket, &clientSocket, &message); // Se il cliente o il pub si disconnettono senza preavviso

                        cout << "Pub: " << message << endl; //Stampa il messaggio

                    }

                    if(message.substr(0, 13).compare("Ordine pronto") == 0){ // Se l'ordine è pronto
                        cout << "Ritiro il menu e lo consegno al tavolo" + ntavolo << endl; // viene ritirato
                        if(!clientSocket.send("Ordine pronto e consegnato")){ // l'ordine viene consegnato al cliente
                            cerr << "Errore nell'invio del messaggio al Pub!" << endl;
                            exit(1); // Termina il processo figlio
                        }
                    }

                    message.clear();

                    if (clientSocket.receive(message) <= 0) { // il camerire riceve ringraziamenti dal cliente
                        cerr << "Errore nella ricezione del messaggio dal client!" << endl;
                        exit(1); // Connessione chiusa o errore
                    }

                    cout << "Cliente: " << message << endl; // Stampa il messaggio del cliente
                }
            }

            cout << "Il cliente ha abbandonato il pub" << endl; // Stampa di successo

            // Avvisa il Pub che il cliente ha lasciato il locale
            if(!remoteSocket.send("Cliente ha liberato il tavolo n: " + to_string(ntavolo))){
                cerr << "Errore nell'invio del messaggio al Pub!" << endl;
                exit(1); // Termina il processo figlio
            }

            exit(0); // Termina il processo figlio
        }
    }

    return 0;
}

/* Funzione di controllo terminazione programma */
void signalHandler(int signum) {
    //chiusura della socket client
    if (clientSocketPtr != nullptr) {
        clientSocketPtr->send("termine_cameriere"); // invia l'avviso al pub
        clientSocketPtr->close(); // chiude la socket
        std::cout << "Client socket closed successfully." << std::endl; // stampa di avviso
    }
    //chiusura della socket server
    if (serverSocketPtr != nullptr) {
        serverSocketPtr->close(); // chiude la socket
        std::cout << "Server socket closed successfully." << std::endl; // stampa di avviso
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

    if (!client->send(message)) { // invia il menu al cliente
        cerr << "Errore nell'invio del menu al cliente" << endl;
        exit(1); // Termina il processo figlio
    }
}

// Funzione del cameriere in caso di SIGNIT
void termine_cameriere(Socket *remoteSocket, Socket *client, string *message) {

    // Se il cliente termina improvvisamente di funzionare
    if (message->compare("termine_cliente") == 0) {
        cout << "Un cliente si è disconnesso improvvisamente!" << endl; // Stampa di errore
        remoteSocket->send(*message); // invia l'avviso al Pub
        client->close(); // chiude la socket
        exit(0); // Termina il programma
    } else if (message->compare("termine_pub") == 0) {
        cout << "Il pub si è disconnesso improvvisamente! Si prega di riavviarlo." << endl; // Stampa di errore
        client->send(*message); // invia l'avviso al cliente
        remoteSocket->close(); // chiude la socket
        exit(0);
    }
}